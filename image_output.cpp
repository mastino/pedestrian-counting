/* image_output.cpp
 * a class to make it easier to switch between saving opencv output
 * and outputing to the screen

 * Brian J Gravelle
 * ix.cs.uoregon.edu/~gravelle
 * gravelle@cs.uoregon.edu

 * Apparently this is something I'm supposed to do:
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.

 * If by some miracle you find this software useful, thanks are accepted in
 * the form of chocolate or introductions to potential employers.

 */

#include "image_output.h"

using namespace cv;
using namespace std;

//@constructor with empty values
//@useage intended to be used inconjunction with the setup function
ImageOutput::ImageOutput(){
  to_file     = false;
  video_count = 0;
  video_list  = NULL;
  name_list   = NULL;
}

//@constructor with specific values
//@memory allocates memory to video_list and name_list
ImageOutput::ImageOutput(bool new_to_file, char** new_name_list, Size new_size, int new_video_count){
  
  setup(new_to_file, new_name_list, new_size, new_video_count);

}

//@destructor
ImageOutput::~ImageOutput(){
  for (int i = 0; i < video_count; i++) {
    delete video_list[i];
  }
  delete[] video_list; 
  delete_char_list(name_list, video_count);
}


//@updates data members and setup video files if neceessary
//@params
//@post if to_file all videoWriters are created or false in returned
//@return true if success false for failure
bool ImageOutput::setup(bool new_to_file, char** new_name_list, Size new_size, int new_video_count){
  //TODO close and destroy existing videos?
  to_file     = new_to_file;
  video_list  = new VideoWriter*[video_count];
  video_count = new_video_count;
  frame_size  = Size(new_size);
  char* file_ext = (char*)".h264";
  bool success = true, is_color = true;
  copy_char_list(new_name_list, new_video_count, name_list);

  if (to_file) {
    int ex = VideoWriter::fourcc('X','2','6','4');    //TODO make more general?

    for (int i = 0; i < video_count; i++) {
      video_list[i] = new VideoWriter(char_cat(name_list[i], file_ext),  ex, 4.0, frame_size, is_color);  
      if(!video_list[i]->isOpened()) {
        cout << "ERROR: video writer didn't open for video " << char_cat(name_list[i], file_ext) << endl;
        cout << "Press enter to continue..." << endl;
        getchar();
        success  = false;
      }
      is_color = false; // only the tracking (first) is in color
    }
  } else {
    namedWindow(name_list[0], CV_WINDOW_NORMAL);
    //TODO check for success?
  }

  return success;
}

//@outputs the requested frames
//TODO make return determine success?
char ImageOutput::output_track_frame(Mat &frame){
  if(to_file) {
    output_one_frame_to_file(frame, 0);
    return 'x';
  } else {
    imshow(name_list[0],frame);
    resizeWindow(name_list[0], WIN_HIEGHT, WIN_LENGTH);
    return waitKey(10);
  }
}
 
bool ImageOutput::output_debug_frames(Mat frames[]){
  for (int i = 1; i < video_count; i++) {
    if(frames[i-1].empty()) {
      cout <<  "ERROR: Could not open or find the image" << std::endl ;
      return false;
    }
    
    if(to_file) {
      output_one_frame_to_file(frames[i-1], i);
    } else {
      namedWindow(name_list[i], CV_WINDOW_NORMAL);
      imshow(name_list[i], frames[i-1]);
      resizeWindow(name_list[i], WIN_HIEGHT, WIN_LENGTH);
    }
  }
  return true;
}

void ImageOutput::output_one_frame_to_file(Mat &frame, int i){
  video_list[i]->set(CAP_PROP_FRAME_WIDTH, frame.size().width);
  video_list[i]->set(CAP_PROP_FRAME_HEIGHT, frame.size().height);
  video_list[i]->write(frame);
}

//@closes output
void ImageOutput::close_track_frame(){
  if(!to_file)
    destroyWindow(name_list[0]);
}

void ImageOutput::close_debug_frames(){
  if(!to_file)  
    for (int i = 1; i < video_count; i++)
      destroyWindow(name_list[i]);
}


//@getters (no setters only initial setup)
bool ImageOutput::get_to_file() const{
  return to_file;
}

int  ImageOutput::get_video_count() const{
  return video_count;
}

