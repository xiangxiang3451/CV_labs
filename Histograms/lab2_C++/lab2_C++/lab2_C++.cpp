#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>    // для функции log
#include <numeric>  // для функции accumulate

using namespace cv;
using namespace std;

// Функция гистограммного выравнивания
Mat histogramEqualization(const Mat& I_channel) {
	// Вычисляем гистограмму
	int histSize = 256; // Количество уровней яркости
	float range[] = { 0, 256 }; // Диапазон значений
	const float* histRange = { range };
	Mat hist;
	calcHist(&I_channel, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false); // Вычисление гистограммы

	// Вычисляем функцию накопления (CDF)
	vector<float> cdf(histSize, 0);
	cdf[0] = hist.at<float>(0);
	for (int i = 1; i < histSize; i++) {
		cdf[i] = cdf[i - 1] + hist.at<float>(i); // Накопление значений гистограммы
	}

	// Нормализуем функцию накопления
	float cdf_min = *min_element(cdf.begin(), cdf.end()); // Находим минимальное значение CDF
	Mat I_eq = I_channel.clone(); // Копируем исходный канал
	for (int i = 0; i < I_channel.rows; i++) {
		for (int j = 0; j < I_channel.cols; j++) {
			if (cdf_min < 1) { // Избегаем деления на ноль
				I_eq.at<uchar>(i, j) = saturate_cast<uchar>((cdf[I_channel.at<uchar>(i, j)] - cdf_min) / (1 - cdf_min) * 255);
			}
			else {
				I_eq.at<uchar>(i, j) = I_channel.at<uchar>(i, j); // Без изменений, если cdf_min слишком высоко
			}
		}
	}

	return I_eq; // Возвращаем выровненный канал
}

// Функция преобразования Релея
Mat rayleighTransform(const Mat& I_eq) {
	Mat result = I_eq.clone(); // Копируем выровненный канал
	double c = 255.0 / log(1.0 + 255.0);  // Вычисляем коэффициент c

	for (int i = 0; i < I_eq.rows; i++) {
		for (int j = 0; j < I_eq.cols; j++) {
			result.at<uchar>(i, j) = saturate_cast<uchar>(c * log(1 + I_eq.at<uchar>(i, j)));
		}
	}

	return result; // Возвращаем результат преобразования Релея
}

// Функция для отображения гистограммы
void showHistogram(const Mat& channel, const string& windowName) {
	int histSize = 256; // Количество уровней яркости
	float range[] = { 0, 256 }; // Диапазон значений
	const float* histRange = { range };
	Mat hist;

	// Вычисляем гистограмму
	calcHist(&channel, 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);

	// Нормализуем гистограмму
	Mat histImage = Mat::zeros(200, 256, CV_8UC1);
	normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	// Рисуем гистограмму
	for (int i = 1; i < histSize; i++) {
		line(histImage, Point(i - 1, 200 - cvRound(hist.at<float>(i - 1))),
			Point(i, 200 - cvRound(hist.at<float>(i))), Scalar(255), 2, 8, 0);
	}

	imshow(windowName, histImage); // Отображаем гистограмму
}

int main() {
	Mat I = imread("E:\\Desktop\\work-space\\CV\\lab2\\Histograms\\lab2_C++\\123.jpg"); // Загружаем изображение
	if (I.empty()) {
		cout << "Ошибка: Не удалось открыть изображение. Пожалуйста, проверьте путь и файл." << endl;
		return -1;
	}

	// Извлекаем каналы BGR
	vector<Mat> channels(3);
	split(I, channels);  // Разделяем изображение на каналы B, G, R

	// Применяем гистограммное выравнивание к каждому каналу
	Mat I_red_eq = histogramEqualization(channels[2]); // Красный
	Mat I_green_eq = histogramEqualization(channels[1]); // Зеленый
	Mat I_blue_eq = histogramEqualization(channels[0]); // Синий

	// Применяем преобразование Релея к каждому каналу
	Mat I_red_stretch = rayleighTransform(I_red_eq);
	Mat I_green_stretch = rayleighTransform(I_green_eq);
	Mat I_blue_stretch = rayleighTransform(I_blue_eq);

	// Объединяем обработанные каналы в цветное изображение
	Mat I_enhanced;
	merge(vector<Mat>{I_blue_stretch, I_green_stretch, I_red_stretch}, I_enhanced);

	// Отображаем оригинальное и улучшенное изображения
	imshow("Оригинальное изображение", I);
	imshow("Улучшенное изображение", I_enhanced);

	// Отображаем оригинальные и улучшенные каналы
	imshow("Оригинальный красный канал", channels[2]);
	imshow("Улучшенный красный канал", I_red_stretch);
	imshow("Оригинальный зеленый канал", channels[1]);
	imshow("Улучшенный зеленый канал", I_green_stretch);
	imshow("Оригинальный синий канал", channels[0]);
	imshow("Улучшенный синий канал", I_blue_stretch);

	// Отображаем гистограммы для каждого канала
	showHistogram(channels[2], "Гистограмма красного канала");
	showHistogram(channels[1], "Гистограмма зеленого канала");
	showHistogram(channels[0], "Гистограмма синего канала");

	waitKey(0); // Ожидание нажатия клавиши
	return 0;
}
