#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>

#define UKUPAN_BROJ_PUTNIKA 10

pthread_mutex_t mutex;
sem_t slobodna_mesta_na_stanici;
sem_t dolazak_busa;
sem_t odlazak_busa;

int putniciKojiCekaju = 0;
int putniciKojiSuOtisli = 0;

void * putnik() {

    sem_wait(&slobodna_mesta_na_stanici);

    pthread_mutex_lock(&mutex);
    putniciKojiCekaju++;
    printf("Broj putnika koji ceka = %d \n",putniciKojiCekaju);
    sleep(1);

    pthread_mutex_unlock(&mutex);

    sem_wait(&dolazak_busa);
    sem_post(&slobodna_mesta_na_stanici);

    printf("Putnik ulazi u bus. Broj putnika koji ceka: %d \n",--putniciKojiCekaju);
    putniciKojiSuOtisli++;
    sleep(1);

    if(putniciKojiCekaju==0) {
        sem_post(&odlazak_busa);
    } else {
        sem_post(&dolazak_busa);
    }

}

void * bus() {

    while(1) {

        pthread_mutex_lock(&mutex);

        if(putniciKojiCekaju > 0) {
            sem_post(&dolazak_busa);
            sem_wait(&odlazak_busa);
        }

        printf("Bus je pun i krece dalje! Ljudi koji su ostali na stanici: %d\n",putniciKojiCekaju);
        pthread_mutex_unlock(&mutex);

        if(putniciKojiSuOtisli == UKUPAN_BROJ_PUTNIKA) break;
        sleep(1);
        
    }

}

int main(void) {

    pthread_t putnici [UKUPAN_BROJ_PUTNIKA];
    pthread_t busNit;
    pthread_mutex_init(&mutex,NULL);

    sem_init(&slobodna_mesta_na_stanici,0,50);
    sem_init(&dolazak_busa,0,0);
    sem_init(&odlazak_busa,0,0);

    pthread_create(&busNit,NULL,bus,NULL);

    for(int i=0; i< UKUPAN_BROJ_PUTNIKA ;i++) {
        pthread_create(&putnici[i],NULL,putnik,NULL);
        sleep(1);
    }

    pthread_join(busNit,NULL);
    printf("Dan je gotov! Svi putnici su otisli svojim kucama!\n");

}