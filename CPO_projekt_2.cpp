#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>


//Funkcja wczytująca ścieżkę do obrazka
std::string getPath()
{
	std::string path;
	std::cout << "Podaj sciezke do obrazka: ";
	std::cin >> path;
	for (int i = 0; i < path.size(); i++)
	{
		//sprawdzenie czy ukosniki w scieżce sa juz odwrocone, czy trzeba odwrócić
		if (path[i] == 92)
		{
			path[i] = '/';
		}
	}
	return path;
}

//funkcja rozpoznająca kolor znajdujący się na karcie poprzez wybranie ROI dookoła środka karty - detekcja odbywa się na podstawie obserwacji wartości kolorów RGB
int detectColor(cv::Mat image)
{
	int cardColor = 0;

	cv::cvtColor(image, image, cv::COLOR_HSV2BGR);

	cv::Scalar color = (0, 0, 0);  // Blue   Green   Red
	color = mean(image);
	//"Pick color: 1-red, 2-green, 3-blue, 4-yellow"
	if (color(2) > 90)
	{
		if (color(2) - color(1) > 40) {
			cardColor = 1; //red
		}
		else
		{
			cardColor = 4; //yellow
		}
	}

	else if (color(2) < 90)
	{
		if (color(0) > 40)
		{
			cardColor = 3; //blue
		}
		else
		{
			cardColor = 2; //green
		}
	}

	return cardColor;
}


/*Funkcja odpowiedzialna za przetwarzanie wstępnie obrazu - niwelacja szumów gradientu, soli i pieprzu oraz bluru, finalnie otrzymujemy wyraźnie widoczne binarne obrazy kart
(i obraz po filtrze medianowym, który jest kolorowy - służy do obliczenia koloru kart*/

void preProcessing(cv::Mat originalImage, cv::Mat& medianBlurred, cv::Mat& thresholdImage)
{
	cv::Mat img = originalImage;

	cv::medianBlur(img, medianBlurred, 5);
	cv::cvtColor(medianBlurred, img, cv::COLOR_BGR2GRAY);
	cv::GaussianBlur(img, img, cv::Size(3, 3), 1.5);

	cv::adaptiveThreshold(img, thresholdImage, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 9, 4.5);

	cv::dilate(thresholdImage, thresholdImage, cv::Mat(), cv::Point(-1, -1), 2);
	cv::erode(thresholdImage, thresholdImage, cv::Mat(), cv::Point(-1, -1), 2);
}

//funkcja znajdująca zewnętrzne kotury kart i zapisująca je w wektorze
void findCardsContours(std::vector<std::vector<cv::Point> > allObjectsExternalContours, std::vector<std::vector<cv::Point> >& cardsExternalContours)
{
	for (int i = 0; i < allObjectsExternalContours.size(); i++)
	{
		if (cv::contourArea(allObjectsExternalContours[i]) > 16000)
			//16000 - oszacowana minimalna wartość pola karty - były one zazwyczaj w granicy 17500-19000, przyjęto wartość niższą, bo inne pola były dużo mniejsze
		{
			cardsExternalContours.push_back(allObjectsExternalContours[i]); //wektor na pojedyncze karty
		}
	}
}

/*funkcja wyznaczająca ROI na obrazie binarnym z widocznym znakiem na środku - do detekcji kart  oraz ROI na obrazie kolorowym - do detekcji koloru karty
   wyznacza też środki kart i zapisuje je do wektora*/
void chooseROI(std::vector<std::vector<cv::Point> > cardsExternalContours, std::vector<cv::Point2d>& centers, std::vector<cv::Mat>& ROIs, cv::Mat thresholdImage, cv::Mat medianBlurred, std::vector<cv::Mat>& ROIsInColor)
{
	for (int i = 0; i < cardsExternalContours.size(); i++)
	{
		auto area = cv::contourArea(cardsExternalContours[i]);
		auto moments = cv::moments(cardsExternalContours[i]);
		auto center = cv::Point2d{ moments.m10 / moments.m00, moments.m01 / moments.m00 };
		centers[i] = center;

		//dobranie odpowiedniego ROI manipulując polem karty i dzielnikiem dobranym na podstawie obserwacji obszaru;
		ROIs[i] = thresholdImage(cv::Range(center.y - area / 800, center.y + area / 800), cv::Range(center.x - area / 800, center.x + area / 800));
		cv::Mat colorROI = medianBlurred(cv::Range(center.y - area / 750, center.y + area / 750), cv::Range(center.x - area / 750, center.x + area / 750));
		cv::cvtColor(colorROI, colorROI, cv::COLOR_BGR2HSV);
		ROIsInColor[i] = colorROI;
	}
}

//funkcja służąca do wyboru koloru przez użytkownika
int chooseColor()
{
	int color = 0;

	std::cout << "Pick color: \n1-red\n2-green\n3-blue\n4-yellow\n5-all cards " << std::endl;
	do {
		std::cin >> color;
	} while (color < 1 || color>5);

	return color;
}


/*Funkcja obliczająca największe pole na obrazie ROI w celu detekcji na nim konturu znaku, który należy rozpoznać - jeżeli pole konturu jest największe, to znak musi być poszukiwaną figurą
 Zwraca też wszystkie kontury aż do napotkania największego konturu*/
double findBiggestArea(std::vector < std::vector<cv::Point> >ROIpoints, std::vector < std::vector<cv::Point> >& contoursUntilMax)
{
	double biggestArea = 0;

	for (int j = 0; j < ROIpoints.size(); j++)
	{
		if (cv::contourArea(ROIpoints[j]) > biggestArea)
		{
			biggestArea = cv::contourArea(ROIpoints[j]);
			contoursUntilMax.push_back(ROIpoints[j]);
		}
	}

	return biggestArea;
}


//Funkcja odpowiedzialna za obliczenie kolistości znaków - jeden z parametrów służący do ich rozpoznania
double calculateCircularity(std::vector < std::vector<cv::Point> >contoursUntilMax, double biggestArea)
{
	double area = biggestArea;
	double arcLenght = cv::arcLength(contoursUntilMax[contoursUntilMax.size() - 1], true);
	double circularity = 4 * CV_PI * area / (arcLenght * arcLenght);

	return circularity;
}

//Funkcja odpowiedzialna za rozpoznanie znaków na podstawie obliczonych wcześniej parametrów i zaobserwowanych zależności i wartości
std::string detectShape(std::vector<cv::Vec3f> circles, double circularity, double biggestArea)
{
	std::string shape;

	if (circles.size() == 2 || (circles.size() == 1 && circularity > 0.35 && circularity < 0.8))
	{
		shape = "3";
	}

	else if (circles.size() == 1 && circularity > 0.8)
	{
		shape = "stop";
	}

	else if (circles.size() == 1 && circularity < 0.35)
	{
		shape = "2";
	}

	else if (circles.size() == 0 && (biggestArea > 3800 && biggestArea < 4900))
	{
		shape = "4";
	}

	else if (circles.size() == 0 && biggestArea < 3800)
	{
		shape = "reverse";
	}

	return shape;
}

int main()
{
	/*****************
	 * PREPROCESSING *
	 ******************/

	cv::Mat originalImage = cv::imread(getPath());
	cv::resize(originalImage, originalImage, cv::Size(1024, 768));

	cv::Mat medianBlurred = cv::Mat::zeros(originalImage.size(), CV_32S);
	cv::Mat thresholdImage = cv::Mat::zeros(originalImage.size(), CV_8U);

	preProcessing(originalImage, medianBlurred, thresholdImage);

	/***************
	 * SEGMENTACJA *
	 * *************/

	 //Oddzielenie konturów zewnętrznych kart od innych znalezionych konturów
	std::vector<std::vector<cv::Point> > allObjectsExternalContours;
	std::vector<std::vector<cv::Point> > cardsExternalContours;

	cv::findContours(thresholdImage, allObjectsExternalContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	findCardsContours(allObjectsExternalContours, cardsExternalContours);

	cv::Mat cardsContours = cv::Mat::zeros(thresholdImage.size(), CV_8U);

	cv::drawContours(cardsContours, cardsExternalContours, -1, cv::Scalar(255, 0, 0));


	//Wyznaczenie środów kart oraz ROI do detekcji kształtów na karcie i kolorów
	std::vector<cv::Point2d> centers(cardsExternalContours.size());
	std::vector<cv::Mat> ROIs(cardsExternalContours.size());
	std::vector<cv::Mat> colorROIs(cardsExternalContours.size());

	std::vector<cv::Mat> ROIsInColor(centers.size());

	chooseROI(cardsExternalContours, centers, ROIs, thresholdImage, medianBlurred, ROIsInColor);

	double biggestArea = 0;
	int sumOfCards = 0;

	//Wybranie koloru przez użytkownika
	int pickColor = chooseColor();

	for (int i = 0; i < ROIs.size(); i++)
	{
		std::string shape;
		int cardColor = detectColor(ROIsInColor[i]);

		//Detekcja wykonuje się jedynie wtedy, gdy dany kolor jest równy kolorowi wybranemu przez użytkownika lub użytkownik wybrał "wszystkie karty"
		if (pickColor != 5) {
			if (cardColor != pickColor) {
				continue;
			}
		}

		/*****************************
		* DETEKCJA KSZTAŁTU I KOLORU *
		* ****************************/

		std::vector < std::vector<cv::Point> >contoursUntilMax;
		std::vector < std::vector<cv::Point> >ROIpoints;

		//znalezienie wszystkich konturów w danym ROI i największego pola
		cv::findContours(ROIs[i], ROIpoints, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

		biggestArea = findBiggestArea(ROIpoints, contoursUntilMax);

		cv::Mat roiContoursImg = cv::Mat::zeros(ROIs[i].size(), CV_8U);
		cv::drawContours(roiContoursImg, contoursUntilMax, -1, cv::Scalar(255, 0, 0));

		//obliczenie kolistości kształtu o największym polu w danym ROI - musi to być szukany znak
		double circularity = calculateCircularity(contoursUntilMax, biggestArea);

		//wpasowanie w daną figurę okręgów o zadanych parametrach - ilość okręgów jest jednnym z parametrów do detekcji znaku
		std::vector<cv::Vec3f> circles;
		HoughCircles(roiContoursImg, circles, cv::HOUGH_GRADIENT, 1, 20, 45, 29, 19, 45);

		//Przypisanie zdetekowanego kształu
		shape = detectShape(circles, circularity, biggestArea);


		//Wykonanie zadanych w wymaganiach projektu działań w zależności od ształtu 
		if (shape == "3")
		{
			sumOfCards += 3;
		}

		else if (shape == "stop")
		{
			std::cout << "detected shape: stop" << std::endl;
			std::cout << "card center coordinates: " << centers[i] << std::endl;
			circle(originalImage, centers[i], 10, cv::Scalar(255, 255, 0));
		}

		else if (shape == "2")
		{
			sumOfCards += 2;
		}

		else if (shape == "4")
		{
			sumOfCards += 4;
		}

		else if (shape == "reverse")
		{
			std::cout << "detected shape: reverse" << std::endl;
			std::cout << "card center coordinates: " << centers[i] << std::endl;
			circle(originalImage, centers[i], 10, cv::Scalar(255, 255, 0));
		}
		biggestArea = 0;
	}

	std::cout << "Sum of cards: " << sumOfCards << std::endl;
	//cv::imshow("final contours", cardsContours);
	cv::imshow("original", originalImage);
	cv::waitKey(0);

	return 0;
}