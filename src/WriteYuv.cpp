#include "WriteYuv.h"

#include <stdio.h>
#include <string.h>
#include <iostream>

#include "FrameQueue.h"

using namespace std;

//播放yuv文件： ffplay -f rawvideo -video_size 1280*720 frame1280x720-0.yuv 本地文件
WriteYuv::WriteYuv()
{
	if (!path_yuv)
	{
		path_yuv = (char*)malloc(strlen(dir_yuv) + strlen(suffix) + 8);
	}
}

WriteYuv::~WriteYuv()
{
	free(path_yuv);
	dir_yuv = nullptr;
	suffix = nullptr;
}

void WriteYuv::StoreYuvData(const GWVideoFrame* yuv_data, const int width, const int height)
{
	auto f_write_file = [=]() {
		sprintf(path_yuv, "%s%dx%d-%d%s", dir_yuv, width, height, count_frame, suffix);
		count_frame++;

		FILE* yuv_fp;
		fopen_s(&yuv_fp, path_yuv, "wb+");
		if (yuv_fp)
		{
			fwrite(yuv_data, 1, width * height * 3 / 2, yuv_fp);
			fclose(yuv_fp);
		}
	};

	std::thread t_write_file(f_write_file);
	t_write_file.detach();
}
