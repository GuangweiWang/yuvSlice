//
//  main.cpp
//  yuvSlice
//
//  Created by Guangwei Wang on 11/24/15.
//  Copyright © 2015 Guangwei Wang. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include "yuvSlice.h"

using namespace std;

//print usage for wrong parameters
void print_usage_and_exit(bool condition,
                          const char* message = "usage", bool manu = true) {
  if(condition) {
    if(manu) {
      cout << "Help: " << message << endl << endl;
      
      cout << "usage:" << endl;
      cout << "./yuvSlice.bin widht height infile.yuv out.yuv " << endl;
      
      cout << "width:        input width(luma samples)" << endl;
      cout << "height:       input height(luma samples)" << endl;
      cout << "infile.yuv:   infile name" << endl;
      cout << "outfile.yuv:  outfile name" << endl;
      cout << endl;
      
      cout << "example:" << endl;
      cout << "./yuvSlice.bin 1280 720 infile.yuv out.yuv" << endl;
      
    }else{
      cout << "parameters " << message << " error!" << endl;
    }
    exit(1);
  }
}

void ParseCmdLine(int argc, const char* argv[], SProcessCtx* pProcessCtx) {
  if (argc != 5) {
    print_usage_and_exit(true, "wrong number of argument");
  }
  
  if (!pProcessCtx) {
    cout << "Error: invalid processCtx" << endl;
    exit(1);
  }
  
  pProcessCtx->iWidth = atoi(argv[1]);
  pProcessCtx->iHeight = atoi(argv[2]);
  pProcessCtx->pInfileName = argv[3];
  pProcessCtx->pOutfileName = argv[4];
  
  if (pProcessCtx->pInfileName == NULL || pProcessCtx->pOutfileName == NULL) {
    cout << "Error: invalid infile/outfile name" << endl;
    exit(1);
  }
  
  if (pProcessCtx->iWidth <= 0 || pProcessCtx->iHeight <= 0) {
    cout << "Error: invalid width or height" << endl;
  }
}

void ReadColorComponent(SColorComponent* pColorComponent, FILE* fin) {
  unsigned char* pdata = pColorComponent->pdata;
  int iWidth = pColorComponent->iWidth;
  int iHeight = pColorComponent->iHeight;
  int iStride = pColorComponent->iStride;
  
  for (int i = 0; i < iHeight; i++) {
    fread(pdata, 1, iWidth, fin);
    pdata += iStride;
  }
}

void WriteColorComponent(SColorComponent* pColorComponent, FILE*fout) {
  unsigned char* pdata = pColorComponent->pdata;
  int iWidth = pColorComponent->iWidth;
  int iHeight = pColorComponent->iHeight;
  int iStride = pColorComponent->iStride;
  
  for (int i = 0; i < iHeight; i++) {
    fwrite(pdata, 1, iWidth, fout);
    pdata += iStride;
  }
}

void ReadFrame(SYUVFrame* sInFrame, FILE* fin) {
  ReadColorComponent(&sInFrame->y, fin);
  ReadColorComponent(&sInFrame->u, fin);
  ReadColorComponent(&sInFrame->v, fin);
}

void WriteFrame(SYUVFrame* sOutFrame, FILE* fout) {
  WriteColorComponent(&sOutFrame->y, fout);
  WriteColorComponent(&sOutFrame->u, fout);
  WriteColorComponent(&sOutFrame->v, fout);
}

void CreatFrame(SYUVFrame* sFrame, SProcessCtx* pProcessCtx) {
  if (pProcessCtx == NULL) {
    cout << "Error: invalid ProcessCtx" << endl;
    exit(1);
  }
  
  if (pProcessCtx->iWidth <= 0 || pProcessCtx->iHeight <= 0) {
    cout << "Error: invalid width or height" << endl;
  }
  
  sFrame->y.iWidth = pProcessCtx->iWidth;
  sFrame->y.iHeight = pProcessCtx->iHeight;
  sFrame->y.iStride = pProcessCtx->iWidth;
  
  sFrame->u.iWidth = sFrame->v.iWidth = sFrame->y.iWidth >> 1;
  sFrame->u.iHeight = sFrame->v.iHeight = sFrame->y.iHeight >> 1;
  sFrame->u.iStride = sFrame->v.iStride = sFrame->y.iStride >> 1;
  
  int iFrameSize = sFrame->y.iWidth * sFrame->y.iStride;
  
  if (sFrame->y.pdata || sFrame->u.pdata || sFrame->v.pdata) {
    cout << "Error: can not creat existed frame" << endl;
    exit(1);
  }
  
  sFrame->y.pdata = (unsigned char*)malloc(iFrameSize);
  sFrame->u.pdata = (unsigned char*)malloc(iFrameSize >> 2);
  sFrame->v.pdata = (unsigned char*)malloc(iFrameSize >> 2);
  
  if (!sFrame->y.pdata || !sFrame->u.pdata || !sFrame->v.pdata) {
    cout << "Error: creat frame error" << endl;
    exit(1);
  }
}

void DestroyFrame(SYUVFrame* sFrame) {
  if (sFrame->y.pdata) {
    delete sFrame->y.pdata;
    sFrame->y.pdata = nullptr;
  }
  
  if (sFrame->u.pdata) {
    delete sFrame->u.pdata;
    sFrame->u.pdata = nullptr;
  }
  
  if (sFrame->v.pdata) {
    delete sFrame->v.pdata;
    sFrame->v.pdata = nullptr;
  }
}

void DuplicateColorComponent(SColorComponent* pDstComponent,
                             SColorComponent* pSrcComponent) {
  unsigned char* pSrcData = pSrcComponent->pdata;
  unsigned char* pDstData = pDstComponent->pdata;
  
  pDstComponent->iWidth = pSrcComponent->iWidth;
  pDstComponent->iHeight = pSrcComponent->iHeight;
  pDstComponent->iStride = pSrcComponent->iStride;
  
  for (int i = 0; i < pDstComponent->iHeight; i++) {
    memcpy(pDstData, pSrcData, pDstComponent->iWidth);
    pDstData += pDstComponent->iStride;
    pSrcData += pSrcComponent->iStride;
  }
}

void DuplicateFrame(SYUVFrame* pDstFrame, SYUVFrame* pSrcFrame) {
  DuplicateColorComponent(&pDstFrame->y, &pSrcFrame->y);
  DuplicateColorComponent(&pDstFrame->u, &pSrcFrame->u);
  DuplicateColorComponent(&pDstFrame->v, &pSrcFrame->v);
}

int main(int argc, const char * argv[]) {
  
  SProcessCtx     sProcessCtx;
  SProcessCtx*    pProcessCtx = &sProcessCtx;
  SYUVFrame       sInFrame;
  SYUVFrame       sOutFrame;
  SYUVFrame*      pInFrame = &sInFrame;
  SYUVFrame*      pOutFrame = &sOutFrame;
  
  FILE*           fin;
  FILE*           fout;
  
  //int             iWidth, iHeight;
  
  memset(pProcessCtx, 0, sizeof(SProcessCtx));
  memset(pInFrame, 0, sizeof(sInFrame));
  memset(pOutFrame, 0, sizeof(sOutFrame));
  
  //parse command line
  ParseCmdLine(argc, argv, pProcessCtx);
  
  fin = fopen(pProcessCtx->pInfileName, "rb");
  if (!fin) {
    cout << "can not open input file" << endl;
    exit(1);
  }
  
  fout = fopen(pProcessCtx->pOutfileName, "wb");
  if (!fout) {
    cout << "can not open output file" << endl;
    exit(1);
  }
  
  //malloc in and out frame
  //cout << "creat frame" << endl;
  CreatFrame(pInFrame, pProcessCtx);
  CreatFrame(pOutFrame, pProcessCtx);
  
  //read frame from file
  //cout << "processing" << endl;
  ReadFrame(pInFrame, fin);
  DuplicateFrame(pOutFrame, pInFrame);
  WriteFrame(pOutFrame, fout);
  
  //end
  DestroyFrame(pInFrame);
  DestroyFrame(pOutFrame);
  fclose(fin);
  fclose(fout);
  
  cout << "success!" << endl;
  
  return 0;
}
