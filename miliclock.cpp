#include <time.h>
#include <stdlib.h>

class miliclock{
    private:
        struct timespec req;
    public:
        miliclock(){
            req={0};
        }
        void sleep(long miliseconds){
            req.tv_sec=miliseconds/1000;
            req.tv_nsec=(long)1000000*(miliseconds%(long)1000);
            nanosleep(&req, NULL); 
        }
        void random_sleep(long min_miliseconds,long max_miliseconds){
            if(min_miliseconds>max_miliseconds){ 
                return;
            }
            srand(time(NULL)); 
            long random = rand();
            long miliseconds=min_miliseconds+(random%(max_miliseconds-min_miliseconds+1));
            sleep(miliseconds);
        }
          
};