#pragma once
#include "FrameQueue.h"

class WriteYuv
{
public:
	WriteYuv();
	virtual ~WriteYuv();

public:
	void StoreYuvData(const GWVideoFrame* yuv_data, const int width, const int height);

private:
	const char* dir_yuv = "D:\\VisualStudioWorkspace\\FFmpegWin\\bin\\frame";
	const char* suffix = ".yuv";
	char* path_yuv = nullptr;
	int count_frame{ 0 };
};

