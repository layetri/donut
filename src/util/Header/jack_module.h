/*
#
# 2017 Marc Groenewegen
# altered by Ciska Vriezenga to serve as a simple example
#
*/

#include "../../Header/Global.h"

#ifdef PLATFORM_DARWIN_X86
  #ifndef _JACK_MODULE_H_
  #define _JACK_MODULE_H_

  #include <string>
  #include <functional>
  #include <jack/jack.h>

  class JackModule
  {
  public:
      JackModule();
      ~JackModule();
      int init();
      int init(std::string clientName);
      unsigned long getSamplerate();
      void autoConnect();
      void end();
      //the onProcess function that needs to be assigned to a JackModule object
      std::function <int(jack_default_audio_sample_t **,
                         jack_default_audio_sample_t *,
                         jack_default_audio_sample_t *,
                         jack_nframes_t)> onProcess;

  private:
      static int _wrap_jack_process_cb(jack_nframes_t nframes,void *arg);
      jack_client_t *client;
      const char **ports;
  };

  #endif
#endif
