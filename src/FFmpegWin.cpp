#include <iostream>

#include <stdio.h>
#include <thread>
#define __STDC_CONSTANT_MACROS

#include "VideoPlayerWin.h"

const char* suffix = ".yuv";
const char* dir_yuv = "D:\\VisualStudioWorkspace\\FFmpeg\\x64\\frame";
char* path_yuv = (char*)malloc(strlen(dir_yuv) + strlen(suffix) + 8);
int count_fp = 0;
char* frame_buf = nullptr;

#include "VideoPlayerWin.h"

std::shared_ptr<VideoPlayerWin> vidoPlayer = nullptr;

void executeFFmpeg()
{
	char file_path[] = "http://clips.vorwaerts-gmbh.de/big_buck_bunny.mp4";
	std::string s_file_Path("D:\\VisualStudioWorkspace\\FFmpegWin\\bin\\Debug\\test-video.mp4");

	vidoPlayer = std::make_shared<VideoPlayerWin>();
	vidoPlayer.get()->SetUrl(s_file_Path);
	vidoPlayer.get()->Start(s_file_Path);

	while (1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void testMemory()
{
	char* test_char_p = (char*)malloc(10);
}

int main(int argc, char* argv[])
{
	
	//std::thread t_ffmpeg(executeFFmpeg);
	//t_ffmpeg.join();
	testMemory();

	_CrtDumpMemoryLeaks();

	return 0;
}