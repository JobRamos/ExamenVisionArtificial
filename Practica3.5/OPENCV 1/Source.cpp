#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <math.h>
#include <cmath>
#include <iostream>

using namespace cv;
using namespace std;

Mat PasarKernelImagen(Mat imagenAmpleada, Mat kernel, int filasImagen, int columnasImagen, int tamKernel);
Mat ExpandirImagen(Mat imagen, int tamKernel);



int main(){
	// kernels Gx y Gy
	Mat kernelGx(3, 3, CV_32F);
	kernelGx.at<float>(Point(0, 0)) = -1;
	kernelGx.at<float>(Point(1, 0)) = 0;
	kernelGx.at<float>(Point(2, 0)) = 1;

	kernelGx.at<float>(Point(0, 1)) = -2;
	kernelGx.at<float>(Point(1, 1)) = 0;
	kernelGx.at<float>(Point(2, 1)) = 2;

	kernelGx.at<float>(Point(0, 2)) = -1;
	kernelGx.at<float>(Point(1, 2)) = 0;
	kernelGx.at<float>(Point(2, 2)) = 1;

	Mat kernelGy(3, 3, CV_32F);
	kernelGy.at<float>(Point(0, 0)) = -1;
	kernelGy.at<float>(Point(1, 0)) = -2;
	kernelGy.at<float>(Point(2, 0)) = -1;

	kernelGy.at<float>(Point(0, 1)) = 0;
	kernelGy.at<float>(Point(1, 1)) = 0;
	kernelGy.at<float>(Point(2, 1)) = 0;

	kernelGy.at<float>(Point(0, 2)) = 1;
	kernelGy.at<float>(Point(1, 2)) = 2;
	kernelGy.at<float>(Point(2, 2)) = 1;

	cout << "\nKenrnelGx\n" << kernelGx << "\n" << endl;
	cout << "\nKenrnelGy\n" << kernelGy << "\n" << endl;


	char NombreImagen[] = "Lenna.png";

	Mat imagen;
	Mat imagenGrises;

	int tamExpancion = 0;
	int tamKernel = 0;
	int centro = 0;
	int x, y;
	int i, j;

	float sigma = 0;
	float resultado = 0;

	// Constantes
	float pi = 3.14159265358979323846;
	float e = 2.71828182845904523536;

	double azul, verde, rojo;

	// Pedimos y validamos un tama�o de kernel impar 
	while (tamKernel % 2 != 1)
	{
		cout << "Ingresa un tama�o impar del kernel" << endl;
		cin >> tamKernel;

		if (tamKernel % 2 == 0) {
			cout << "Tama�o de kernel no valido\n" << endl;
		}
	}

	cout << "Ingresa el valor de sigma" << endl;
	cin >> sigma;
	
	// Creamos kernel
	Mat kernel(tamKernel, tamKernel, CV_32F); // Creamos la matriz de tama�o dinamico 
	centro = (tamKernel - 1) / 2; // Para dimensiones impares
	cout << "Centro: " << centro << "\n" << endl;

	// Llenamos el kernel para Gauss
	cout << "\nCoordenadas kernel Gauss\n" << endl;
	for (i = 0; i < tamKernel; i++)
	{
		y = -1 * (i - centro);

		for (j = 0; j < tamKernel; j++)
		{
			x = j - centro;

			resultado = (1 / (2 * pi * sigma * sigma)) * pow(e, -((x * x + y * y) / (2 * sigma * sigma)));
			kernel.at<float>(Point(j, i)) = resultado;

			cout << "\t(" << x << "," << y << ")";
		}
		cout << endl;
	 }
	cout << "\nValores kernel Gauss\n" << endl;
	cout << kernel << "\n" << endl;
	
	imagen = imread(NombreImagen); // Lectura de la imagen
	if (!imagen.data)
	{
		cout << "Error al cargar la imagen: " << NombreImagen << endl;
		exit(1);
	}
	
	int filasImagen = imagen.rows; // Lectura de cuantas filas tiene la imagen
	int columnasImagen = imagen.cols; // Lectura de cuantas columnas tiene la imagen

	// Pasamos a escala de grises
	//cvtColor(imagen, imagenGrises, COLOR_BGR2GRAY); // Funcion para pasar a escala de grises con openCV
	// Mat imagenGrisesPromedio(filasOriginal, columnasOriginal, CV_8UC1); // CV_8UC1 Dato de tipo uchar de un solo canal
	Mat imagenGrisesNTSC(filasImagen, columnasImagen, CV_8UC1);
	for (i = 0; i < filasImagen; i++)
	{
		for (j = 0; j < columnasImagen; j++)
		{
			azul = imagen.at<Vec3b>(Point(j, i)).val[0];  // B
			verde = imagen.at<Vec3b>(Point(j, i)).val[1]; // G
			rojo = imagen.at<Vec3b>(Point(j, i)).val[2];  // R

			// imagenGrisesPromedio.at<uchar>(Point(j, i)) = uchar((rojo + verde + azul) / 3);
			imagenGrisesNTSC.at<uchar>(Point(j, i)) = uchar(0.299 * rojo + 0.587 * verde + 0.114 * azul);
		}
	}
	// cout << imagenGrisesNTSC << "\n" << endl;
	cout << "Tama�o imagen grises: " << imagenGrisesNTSC.rows * imagenGrisesNTSC.cols << endl;

	// Expandimos imagen
	Mat imagenAmpleada = ExpandirImagen(imagenGrisesNTSC, tamKernel);

	// Aplicamos filtro Gauss
	float sumKernel = 0;
	for (int i = 0; i < tamKernel; i++)
	{
		for (int j = 0; j < tamKernel; j++)
		{
			sumKernel += kernel.at<float>(Point(j, i));
		}
	}
	//cout << "Suma kernel: " << sumKernel << "\n" << endl;

	Mat imagenFiltroGauss = PasarKernelImagen(imagenAmpleada, kernel, filasImagen, columnasImagen, tamKernel) / sumKernel;
	cout << "Tama�o imagen filtro Gauss: " << imagenFiltroGauss.rows * imagenFiltroGauss.cols << endl;

	//Mat imagenEqualizada;
	//cv::equalizeHist(imagenFiltroGauss, imagenEqualizada);
	// cout << imagenFiltroGauss << "\n" << endl;

	// sobel

	
	// Expandimos imagen
	Mat imagenAmpleada2 = ExpandirImagen(imagenFiltroGauss, 3);

	// Pasamos los dos kernel por la imagen
	Mat matrizGx = PasarKernelImagen(imagenAmpleada2, kernelGx, imagenAmpleada2.rows, imagenAmpleada2.cols, 3);
	Mat matrizGy = PasarKernelImagen(imagenAmpleada2, kernelGy, imagenAmpleada2.rows, imagenAmpleada2.cols, 3);

	// Sacamos la matriz |G| y angulos teta
	Mat Gfinal(imagenFiltroGauss.rows, imagenFiltroGauss.cols, CV_8UC1);
	Mat teta(imagenFiltroGauss.rows, imagenFiltroGauss.cols, CV_8UC1);

	Mat nmSuppression(imagenFiltroGauss.rows, imagenFiltroGauss.cols, CV_8UC1);
	double angulo = 0;
	double q = 0;
	double r = 0	;

	double lowThresholdRatio = 0.05;
	double  highThresholdRatio = 0.26;

	double min, max;
	cv::minMaxLoc(nmSuppression, &min, &max);
	double highThreshold = max * highThresholdRatio;
	double lowThreshold = highThreshold * lowThresholdRatio;
	Mat matThreshold(imagenFiltroGauss.rows, imagenFiltroGauss.cols, CV_8UC1);

	int  weak = 25;
	int strong = 255;

	for (int i = 0; i < imagenFiltroGauss.rows ; i++) {
		for (int j = 0; j < imagenFiltroGauss.cols ; j++) {
			Gfinal.at<uchar>(Point(j, i)) = sqrt(pow(matrizGx.at<uchar>(Point(j, i)), 2) + pow(matrizGy.at<uchar>(Point(j, i)), 2));
			angulo = atan2(matrizGx.at<uchar>(Point(j, i)), matrizGy.at<uchar>(Point(j, i))) * 180 / pi;
			if (180 < angulo) {
				angulo = angulo - 180;
				teta.at<float>(Point(j, i)) = angulo;
			}
			else {
				teta.at<float>(Point(j, i)) = angulo;
			}
			

			//angle 0
			if ((0 <= angulo < 22.5) || (157.5 <= angulo <= 180)) {
				q = Gfinal.at<uchar>(Point(j + 1, i));
				r = Gfinal.at<uchar>(Point(j - 1, i));
				//angle 45
			}
			else if ((22.5 <= angulo < 67.5)) {
				q = Gfinal.at<uchar>(Point(j - 1, i + 1));
				r = Gfinal.at<uchar>(Point(j + 1, i - 1));
				//angle 90
			}
			else if ((67.5 <= angulo < 112.5)) {
				q = Gfinal.at<uchar>(Point(j, i + 1));
				r = Gfinal.at<uchar>(Point(j, i - 1));
				//angle 135
			}
			else if (112.5 <= angulo < 157.5) {
				q = Gfinal.at<uchar>(Point(j - 1, i - 1));
				r = Gfinal.at<uchar>(Point(j + 1, i + 1));
			}


			if ((Gfinal.at<uchar>(Point(j, i)) >= q) && (Gfinal.at<uchar>(Point(j, i)) >= r)) {
				nmSuppression.at<uchar>(Point(j, i)) = Gfinal.at<uchar>(Point(j, i));
			}
			else {
				nmSuppression.at<uchar>(Point(j, i)) = 0;
			}

		}
	}
	cout << "Tama�o imagen Gfinal: " << Gfinal.rows * Gfinal.cols << endl;
	cout << "Tama�o imagen imagenFiltroGauss: " << imagenFiltroGauss.rows * imagenFiltroGauss.cols << endl;

	for (int i = 1; i < imagenFiltroGauss.rows - 1; i++) {
		for (int j = 1; j < imagenFiltroGauss.cols - 1; j++) {


			//threshold
			if (nmSuppression.at<uchar>(Point(j, i)) >= highThreshold) {
				matThreshold.at<uchar>(Point(j, i)) = strong;
			}
			if (nmSuppression.at<uchar>(Point(j, i)) <= highThreshold && nmSuppression.at<uchar>(Point(j, i)) >= lowThreshold) {
				matThreshold.at<uchar>(Point(j, i)) = weak;

				//Edge Tracking by Hysteresis
				if (nmSuppression.at<uchar>(Point(j + 1, i - 1)) == strong || nmSuppression.at<uchar>(Point(j + 1, i)) == strong
					|| nmSuppression.at<uchar>(Point(j + 1, i + 1)) == strong || nmSuppression.at<uchar>(Point(j, i - 1)) == strong
					|| nmSuppression.at<uchar>(Point(j, i + 1)) == strong || nmSuppression.at<uchar>(Point(j - 1, i - 1)) == strong
					|| nmSuppression.at<uchar>(Point(j - 1, i)) == strong || nmSuppression.at<uchar>(Point(j - 1, i + 1)) == strong) {
					matThreshold.at<uchar>(Point(j, i)) = strong;
				}
				else {
					matThreshold.at<uchar>(Point(j, i)) = 0;
				}

			}
			if (nmSuppression.at<uchar>(Point(j, i)) <= lowThreshold) {
				matThreshold.at<uchar>(Point(j, i)) = 0;
			}


		}
	}

	cout << "Tama�o imagen nmSuppression: " << nmSuppression.rows * nmSuppression.cols << endl;
	cout << "Tama�o imagen matThreshold: " << matThreshold.rows * matThreshold.cols << endl;

	

	namedWindow("Imagen original", WINDOW_AUTOSIZE); // Creaci�n de una ventana
	imshow("Imagen original", imagen);

	namedWindow("Imagen escala de grises (NTSC)", WINDOW_AUTOSIZE);
	imshow("Imagen escala de grises (NTSC)", imagenGrisesNTSC);

	namedWindow("ImagenaAmpleada", WINDOW_AUTOSIZE);
	imshow("Imagen ampleada", imagenAmpleada);

	/*namedWindow("ImagenaAmpleada2", WINDOW_AUTOSIZE);
	imshow("Imagen ampleada 2", imagenAmpleada2);*/

	namedWindow("Imagen filtro gauss", WINDOW_AUTOSIZE);
	imshow("Imagen filtro gauss", imagenFiltroGauss);

	//namedWindow("Imagen imagenEqualizada", WINDOW_AUTOSIZE);
	//imshow("Imagen imagenEqualizada", imagenEqualizada);


	namedWindow("ImagenGX", WINDOW_AUTOSIZE);
	imshow("Imagen GX", matrizGx);

	namedWindow("ImagenGY", WINDOW_AUTOSIZE);
	imshow("Imagen GY", matrizGy);

	namedWindow("Imagen filtro sobel", WINDOW_AUTOSIZE);
	imshow("Imagen filtro sobel", Gfinal);

	namedWindow("Imagen nmSuppression", WINDOW_AUTOSIZE);
	imshow("Imagen nmSuppression", nmSuppression);

	namedWindow("Imagen matThreshold", WINDOW_AUTOSIZE);
	imshow("Imagen matThreshold", matThreshold);

	waitKey(0);
	
	return 1;
}

Mat PasarKernelImagen(Mat imagenAmpleada, Mat kernel, int filasImagen, int columnasImagen, int tamKernel)
{
	/*cout << "\imagenAmpleada\n" << imagenAmpleada << "\n" << endl;
	cout << "\kernel\n" << kernel << "\n" << endl;*/

	Mat imagenFiltro(filasImagen, columnasImagen, CV_8UC1);
	float resultadoKernel = 0;
	int indexAmpleadai = 0;
	int indexAmpleadaj = 0;

	for (int i = 0; i < filasImagen; i++)
	{
		for (int j = 0; j < columnasImagen; j++)
		{
			// Recorrer kernel 
			resultadoKernel = 0;
			for (int k = 0; k < tamKernel; k++)
			{
				for (int l = 0; l < tamKernel; l++)
				{
					indexAmpleadai = i + k;
					indexAmpleadaj = j + l;
					resultadoKernel += imagenAmpleada.at<uchar>(Point(indexAmpleadaj, indexAmpleadai)) * kernel.at<float>(Point(l, k));
					// cout << resultadoKernel << endl;
				}
			}
			imagenFiltro.at<uchar>(Point(j, i)) = abs(int(resultadoKernel));
		}
	}

	return imagenFiltro;
}

Mat ExpandirImagen(Mat imagen, int tamKernel)
{
	// Expandimos imagen
	int tamExpancion = ((tamKernel - 1) / 2) * 2;
	cout << "Numero de filas y columnas a expandir: " << tamExpancion << "\n" << endl;

	int filasImagen = imagen.rows; // Lectura de cuantas filas tiene la imagen
	int columnasImagen = imagen.cols; // Lectura de cuantas columnas tiene la imagen
	int filasImagenAmpleada = filasImagen + tamExpancion;
	int columnasImagenAmpleada = filasImagen + tamExpancion;

	cout << "Tama�o imagen original: " << filasImagen * columnasImagen << endl;
	cout << "Tama�o imagen ampleada: " << filasImagenAmpleada * columnasImagenAmpleada << "\n" << endl;

	// Creamos una nueva imagen 
	Mat imagenAmpleada(filasImagenAmpleada, columnasImagenAmpleada, CV_8UC1);
	for (int i = 0; i < filasImagenAmpleada; i++) // Inicializamos la imagen ampleada con 0s
	{
		for (int j = 0; j < columnasImagenAmpleada; j++)
		{
			imagenAmpleada.at<uchar>(Point(j, i)) = uchar(0);
		}
	}

	// Pasamos la imagen dejando los espacios de la expancion
	for (int i = 0; i < filasImagen; i++)
	{
		for (int j = 0; j < columnasImagen; j++)
		{
			imagenAmpleada.at<uchar>(Point(j + (tamExpancion / 2), i + (tamExpancion / 2))) = imagen.at<uchar>(Point(j, i));
		}
	}
	// cout << imagenAmpleada << "\n" << endl;
	//cout << "Columnas imagen ampleada: " << columnasImagenAmpleada << "\n" << "Columnas imagen original: " << columnasImagen << "\n" << endl;

	return imagenAmpleada;
}

