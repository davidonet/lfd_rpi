    #include <iostream>
    #include <curses.h>
    #include <unistd.h>
    #include <stdint.h>
    #include <cstdlib>

    #include <cmath>

    #include "RtAudio.h"
    #include "texts.h"

    //https://push.cx/2008/256-color-xterms-in-ubuntu
    //sudo dpkg-reconfigure console-setup (sur la rpi)


    RtAudio adc;
    bool screenready = false;
    const char * text="The output display should default to HDMI if an HDMI display is connected, and composite if not (the composite output is the yellow RCA phono connector)";
    int frame = 0;
    int line = 0;
    int c = 1;
    int record( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                double streamTime, RtAudioStreamStatus status, void *userData ) 
    {
        if (screenready) {
            //  if ( status )
            //    std::cout << "Stream overflow detected!" << std::endl;

            int width,height;
            int16_t * data = (int16_t *) inputBuffer;
            erase();
                getmaxyx(stdscr,height,width);
                if(frame == 0){
                    line = (line+1)%(73-height);
                }
                frame = (frame + 1 ) % 100; // si tu remplaces 80 par 100 les lignes de texte defile moins vite.
                for (int x = 0; x < width; ++x) {

                    int h = ((int)data[nBufferFrames*x/width]*height/2)/32768+height/2;

                    int d = h > height/2 ? 1 : -1;
                    for (int y = height/2+1; y != h ; y += d) {

                		
                        if( (rand() % 100) < 30 ){ // 40/100 chances d'avoir un caractere du texte
                            attroff(COLOR_PAIR(2));
                            mvaddch(y,x,texts[y+line][x]);
                        }
                        else {
                            attron(COLOR_PAIR(c));
                            mvaddch(y,x,' ');
                        }
                    }
                       
                        if( (rand() % 100) < 50 ){
                            attron(COLOR_PAIR(1));
                            mvaddch(h,x,'0');
                        }
                        else
                            {
                                attron(COLOR_PAIR(3));
                         mvaddch(h,x,'1');       
                            }

                }

            move(0,0);
            refresh();			/* Print it on to the real screen */

        }
        return 0;
    }


    void initaudio() {

        using namespace std;

        /*
        vector<RtAudio::Api> apis;
        RtAudio::getCompiledApi(apis);
        cout << apis.size() << endl;
        for (size_t i = 0; i < apis.size(); ++i) {
            cout << apis[i] << endl;
        }
    */
        int ndevices = adc.getDeviceCount();
        if ( ndevices < 1 ) {
            std::cout << "\nNo audio devices found!\n";
            return;
        }


        cout << endl;
        for (int i = 0; i < ndevices; ++i) {
            RtAudio::DeviceInfo devinfo;
            devinfo = adc.getDeviceInfo(i);
            cout << i << " " << devinfo.name << " " << devinfo.inputChannels << endl;
        }


        RtAudio::StreamParameters parameters;
        parameters.deviceId = 2;
        parameters.nChannels = 1;
        parameters.firstChannel = 0;
        unsigned int sampleRate = 1200;
        unsigned int bufferFrames = 8; // 256 sample frames
        try {
            adc.openStream( NULL, &parameters, RTAUDIO_SINT16,
                            sampleRate, &bufferFrames, &record );
            adc.startStream();
        }
        catch ( exception& e ) {
            std::cout << "EXCEPTION" << endl << e.what();
            return;
        }

    }


    int main()
    {

        initaudio();

        initscr();			/* Start curses mode 		  */
        if(has_colors() == FALSE)
        {	endwin();
            printf("Your terminal does not support color\n");
            return(-1);
        }

        start_color();
        init_pair(1, COLOR_WHITE, COLOR_YELLOW);
        init_pair(2, COLOR_WHITE, COLOR_BLUE);
        init_pair(3, COLOR_WHITE, COLOR_GREEN);
        init_pair(4, COLOR_WHITE, COLOR_RED);
        init_pair(5, COLOR_WHITE, COLOR_MAGENTA);
        init_pair(6, COLOR_WHITE, COLOR_CYAN);
        init_pair(7, COLOR_WHITE, COLOR_WHITE);


        noecho();
        cbreak();
        screenready = true;
        while(c=getch()){
            if(c == 'q')
                exit(0);
            c = c-'0';
        }
        endwin();			/* End curses mode		  */
        return 0;
    }

