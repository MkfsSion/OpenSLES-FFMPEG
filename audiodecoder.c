#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <audiodecoder.h>

static AVFormatContext *pFormatContext = NULL;
static AVCodec *pCodec = NULL;
static AVCodecContext *pCodecContext = NULL;
static AVCodecParameters *pCodecParameters = NULL;
static int8_t audioStream = -1;
static SwrContext *pSwrContext = NULL;
static AVPacket *pPacket = NULL;
static AVFrame *pFrame = NULL;
static uint8_t *internalbuffer = NULL;

int CreateDecoder(const char *filepath,AudioInfo *infos)
{
  av_register_all();
  pFormatContext = avformat_alloc_context();
  int r=0;
  r=avformat_open_input(&pFormatContext, filepath, NULL, NULL);
  if (r<0){
    ReleaseResources();
    printf("Error:Can not open file(code %d).\n",r);
    return -1; // Fail to open file.
  }

  av_log_set_level(AV_LOG_QUIET);
  if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
    ReleaseResources();
    return -1; // Fail to find stram info;
  }
  for (uint32_t i=0;i<pFormatContext->nb_streams;i++)
  {
    if (pFormatContext->streams[i]->codecpar->codec_type ==
        AVMEDIA_TYPE_AUDIO) {
      audioStream = i;
    }
  }
  if (audioStream == -1) {
    ReleaseResources();
    return -1; // No audio stream found.
  }
  pCodecParameters = pFormatContext->streams[audioStream]->codecpar;
  pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
  if (pCodec == NULL) {
    ReleaseResources();
    return -1; // No codec found.
  }
  pCodecContext = avcodec_alloc_context3(pCodec);
  avcodec_parameters_to_context(pCodecContext,
                                (const AVCodecParameters *)pCodecParameters);
  if (avcodec_open2(pCodecContext, (const AVCodec *)pCodec, NULL) < 0) {
    ReleaseResources();
    return -1; // Fail ro open codec.
  }
  pSwrContext = swr_alloc();
  pSwrContext = swr_alloc_set_opts(
      pSwrContext, pCodecContext->channel_layout, AV_SAMPLE_FMT_S16,
    pCodecContext->sample_rate, pCodecContext->channel_layout,
      pCodecContext->sample_fmt, pCodecContext->sample_rate, 0, NULL);
  swr_init(pSwrContext);
  infos->channels = pCodecContext->channels;
  infos->samplerate = pCodecContext->sample_rate;
  infos->duration = ((pFormatContext->duration) * 1000) / AV_TIME_BASE;
  return 0;
}
  int getAudioSource(void **buffer,size_t *buffersize){
   pPacket = av_packet_alloc();
   pFrame = av_frame_alloc();
   while(av_read_frame(pFormatContext,pPacket)>=0)
      {
    if (pPacket->stream_index == audioStream) {
      int ret = avcodec_send_packet(pCodecContext, pPacket);
      if (ret < 0) {
        return -1; // Fail to decode frame.
      }
      ret = avcodec_receive_frame(pCodecContext, pFrame);
      if (ret < 0) {
        return -1; // Fail to decode frame.
      }
      size_t sizes = av_samples_get_buffer_size(NULL, pFrame->channels,pFrame->nb_samples,AV_SAMPLE_FMT_S16, 1);
      /*printf("buffer size:%lu\n",sizes);
      printf("linesize:%d\n",pFram);*/
      /*printf("Fun buffer size:%lu\n",nsizes;;*/
      if (internalbuffer != NULL) {
        av_free(internalbuffer);
        internalbuffer = NULL;
      }
      internalbuffer = av_malloc(sizeof(uint8_t)*sizes);
          swr_convert(pSwrContext, &internalbuffer, pFrame->nb_samples,
                   (const uint8_t **)pFrame->extended_data,
                   pFrame->nb_samples);
      *buffer = internalbuffer;
      *buffersize = sizes;
      av_packet_unref(pPacket);
      av_frame_unref(pFrame);
      pPacket = NULL;
      pFrame = NULL;
      return 0;
    }
   }
   return -1;
  }
  void ReleaseResources(void) {
    audioStream = -1;
    if (pFrame != NULL) {
      av_frame_unref(pFrame);
      pFrame = NULL;
    }
    if (pPacket != NULL) {
      av_packet_unref(pPacket);
      pPacket = NULL;
    }
    if (pSwrContext != NULL) {
      swr_free(&pSwrContext);
      pSwrContext = NULL;
    }
    if (internalbuffer != NULL) {
      av_free(internalbuffer);
      internalbuffer = NULL;
    }
    if (pCodecParameters != NULL) {
      pCodecParameters = NULL;
    }
    avcodec_close(pCodecContext);
    if (pCodec != NULL) {
      pCodec = NULL;
    }
    if (pCodecContext != NULL) {
      avcodec_free_context(&pCodecContext);
      pCodecContext = NULL;
    }
    avformat_close_input(&pFormatContext);
    if (pFormatContext != NULL) {
      avformat_free_context(pFormatContext);
      pFormatContext = NULL;
    }
  }
