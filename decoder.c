#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

static AVFormatContext *pFormatContext;
static AVCodec *pCodec;
static AVCodecContext *pCodecContext;
static  AVCodecParameters *pCodecParameters;
static int8_t audioStream = -1;
static SwrContext *swrContext;
static AVPacket *pPacket;
static AVFrame *pFrame;
uint8_t *buffer;
int FFMPEGInit(int*channel,int* samplerate,int* duration)
{
  av_register_all();
  const char *file = "";
  pFormatContext = avformat_alloc_context();
  if (avformat_open_input(&pFormatContext, file, NULL, NULL) < 0) {
    return -1; // Fail to open file.
  }
  if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
    return -1; // Fail to find stram info;
  }
  uint64_t udr = pFormatContext->duration;
  *duration = udr / AV_TIME_BASE;
  av_dump_format(pFormatContext, 0, file, 0);
  for (int i=0;i<pFormatContext->nb_streams;i++)
  {
    if (pFormatContext->streams[i]->codecpar->codec_type ==
        AVMEDIA_TYPE_AUDIO) {
      audioStream = i;
    }
  }
  if (audioStream == -1) {
    return -1; // No audio stream found.
  }
  pCodecParameters = pFormatContext->streams[audioStream]->codecpar;
  pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
  if (pCodec == NULL) {
    return -1; // No codec found.
  }
  pCodecContext = avcodec_alloc_context3(pCodec);
  avcodec_parameters_to_context(pCodecContext,
                                (const AVCodecParameters *)pCodecParameters);
  if (avcodec_open2(pCodecContext, (const AVCodec *)pCodec, NULL) < 0) {
    return -1; // Fail ro open codec.
  }
  swrContext = swr_alloc();
  swrContext = swr_alloc_set_opts(
      swrContext, pCodecContext->channel_layout, AV_SAMPLE_FMT_S16,
    pCodecContext->sample_rate, pCodecContext->channel_layout,
      pCodecContext->sample_fmt, pCodecContext->sample_rate, 0, NULL);
 swr_init(swrContext);
  pPacket = malloc(sizeof(AVPacket));
  av_init_packet(pPacket);
  pFrame = av_frame_alloc();
  int buffsize = 192000;
  buffer = malloc(2 * buffsize);
  *channel = pCodecContext->channels;
  *samplerate = pCodecContext->sample_rate;
  return 0;
}
  int getSource(void** data,size_t* datasize){
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
            int writesize =
              av_samples_get_buffer_size(NULL, pFrame->channels,
                pFrame->nb_samples,
                                          pCodecContext->sample_fmt, 1);
          swr_convert(swrContext, &buffer, writesize,
                   (const uint8_t **)pFrame->extended_data,
                   pFrame->nb_samples);
      *data = buffer;
      *datasize = writesize;
      return 0;
    }
   }
   return -1;
  }
  void ReleaseFFMPEGResource(void) {
    av_frame_free(&pFrame);
    av_packet_free(&pPacket);
    swr_free(&swrContext);
    free(buffer);
    avcodec_parameters_free(&pCodecParameters);
    avcodec_close(pCodecContext);
    avformat_close_input(&pFormatContext);
  }
        		
