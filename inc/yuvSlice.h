//
//  yuvSlice.h
//  yuvSlice
//
//  Created by Guangwei Wang on 11/24/15.
//  Copyright © 2015 Guangwei Wang. All rights reserved.
//

#ifndef yuvSlice_h
#define yuvSlice_h

typedef struct {
  const char* pInfileName;
  const char* pOutfileName;
  int iWidth, iHeight;
  int iFrameNum;
}SProcessCtx;

//frame struct
typedef struct {
  unsigned char* pdata;
  int iWidth;
  int iHeight;
  int iStride;
}SColorComponent;

typedef struct {
  SColorComponent y;
  SColorComponent u;
  SColorComponent v;
}SYUVFrame;




#endif /* yuvSlice_h */
