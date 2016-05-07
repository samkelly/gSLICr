// Copyright 2014-2015 Isis Innovation Limited and the authors of gSLICr

#include <time.h>
#include <stdio.h>

#include "gSLICr_Lib/gSLICr.h"
#include "NVTimer.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"



using namespace std;
using namespace cv;

void load_image(const Mat& inimg, gSLICr::UChar4Image* outimg)
{
	gSLICr::Vector4u* outimg_ptr = outimg->GetData(MEMORYDEVICE_CPU);

	for (int y = 0; y < outimg->noDims.y;y++)
		for (int x = 0; x < outimg->noDims.x; x++)
		{
			int idx = x + y * outimg->noDims.x;
			outimg_ptr[idx].b = inimg.at<Vec3b>(y, x)[0];
			outimg_ptr[idx].g = inimg.at<Vec3b>(y, x)[1];
			outimg_ptr[idx].r = inimg.at<Vec3b>(y, x)[2];
		}
}

void load_image(const gSLICr::UChar4Image* inimg, Mat& outimg)
{
	const gSLICr::Vector4u* inimg_ptr = inimg->GetData(MEMORYDEVICE_CPU);

	for (int y = 0; y < inimg->noDims.y; y++)
		for (int x = 0; x < inimg->noDims.x; x++)
		{
			int idx = x + y * inimg->noDims.x;
			outimg.at<Vec3b>(y, x)[0] = inimg_ptr[idx].b;
			outimg.at<Vec3b>(y, x)[1] = inimg_ptr[idx].g;
			outimg.at<Vec3b>(y, x)[2] = inimg_ptr[idx].r;
		}
}


int main(int argc, char** argv)
{
	if(argc != 3) {
		cout << "usage: segmenter [input image path] [output image path]" << endl;
		cout << "args: " << argc << endl;
		return -1;
	}
	char *input_image_path = argv[1];
	char *output_image_path = argv[2];
	cout << "input file: " << argv[1] << endl;
	cout << "output file: " << argv[2] << endl;

	cout << "loading input image..." << endl;
	Mat input_image = imread(input_image_path, 1);
	if(input_image.data == NULL) {
		cout << "error loading input image!" << endl;
		return -1;
	}

	// gSLICr settings
	gSLICr::objects::settings my_settings;
	my_settings.img_size.x = input_image.cols;
	my_settings.img_size.y = input_image.rows;
	my_settings.no_segs = 30;
	my_settings.spixel_size = 16;
	my_settings.coh_weight = 0.6f;
	my_settings.no_iters = 5;
	my_settings.color_space = gSLICr::RGB; // gSLICr::CIELAB for Lab, or gSLICr::RGB for RGB
	my_settings.seg_method = gSLICr::GIVEN_SIZE; // or gSLICr::GIVEN_NUM for given number
	my_settings.do_enforce_connectivity = true; // wheter or not run the enforce connectivity step

	// instantiate a core_engine
	gSLICr::engines::core_engine* gSLICr_engine = new gSLICr::engines::core_engine(my_settings);

	// gSLICr takes gSLICr::UChar4Image as input and out put
	gSLICr::UChar4Image* in_img = new gSLICr::UChar4Image(my_settings.img_size, true, true);
	gSLICr::UChar4Image* out_img = new gSLICr::UChar4Image(my_settings.img_size, true, true);

	cout << "copying input image into matrix..." << endl;
	load_image(input_image, in_img);

	// perform segmentation
	cout << "performing segmentation..." << endl;
	gSLICr_engine->Process_Frame(in_img);
	cout << "drawing segmentation result" << endl;
	gSLICr_engine->Draw_Segmentation_Result(out_img);
	cout << "copying segmentation result to out image..." << endl;
	gSLICr_engine->Write_Seg_Res_To_PGM(output_image_path);
	Size s(my_settings.img_size.x, my_settings.img_size.y);
	Mat boundary_draw_frame;
	boundary_draw_frame.create(s, CV_8UC3);
	load_image(out_img, boundary_draw_frame);
	imwrite(output_image_path, boundary_draw_frame);
	//namedWindow("segmentation", CV_WINDOW_AUTOSIZE );
	//imshow("segmentation", out_image);

	//waitKey(0);
	/*while (cap.read(oldFrame))
	{
		resize(oldFrame, frame, s);

		load_image(frame, in_img);

        sdkResetTimer(&my_timer); sdkStartTimer(&my_timer);

        sdkStopTimer(&my_timer);
        cout<<"\rsegmentation in:["<<sdkGetTimerValue(&my_timer)<<"]ms"<<flush;

		gSLICr_engine->Draw_Segmentation_Result(out_img);

		load_image(out_img, boundry_draw_frame);
		imshow("segmentation", boundry_draw_frame);

		key = waitKey(1);
		if (key == 27) break;
		else if (key == 's')
		{
			char out_name[100];
			sprintf(out_name, "seg_%04i.pgm", save_count);
			gSLICr_engine->Write_Seg_Res_To_PGM(out_name);
			sprintf(out_name, "edge_%04i.png", save_count);
			imwrite(out_name, boundry_draw_frame);
			sprintf(out_name, "img_%04i.png", save_count);
			imwrite(out_name, frame);
			printf("\nsaved segmentation %04i\n", save_count);
			save_count++;
		}
	}*/

	//destroyAllWindows();
    return 0;
}
