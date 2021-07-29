#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <../raspi/sysroot/usr/include/stdio.h>
#include <../raspi/sysroot/usr/include/stdlib.h>
#include <../raspi/sysroot/usr/include/stdint.h>
#include <../raspi/sysroot/usr/include/unistd.h>
#include <../raspi/sysroot/usr/include/math.h>
#include <wiringPi.h>

#define MAX_TIMINGS	85
#define DHT_PIN		27	/* GPIO-27 */
int data[5] = { 0, 0, 0, 0, 0 };
int i;
double h,c,f;
void read_dht_data()
{
    uint8_t laststate	= HIGH;
    uint8_t counter		= 0;
    uint8_t j			= 0, i;
    data[0] = data[1] = data[2] = data[3] = data[4] = 0;
    /* pull pin down for 18 milliseconds */
    pinMode( DHT_PIN, OUTPUT );
    digitalWrite( DHT_PIN, LOW );
    delay( 18 );
    /* prepare to read the pin */
    pinMode( DHT_PIN, INPUT );
    /* detect change and read data */
    for(i = 0; i < MAX_TIMINGS; i++ )
    {
        counter = 0;
        while ( digitalRead( DHT_PIN ) == laststate )
        {
            counter++;
            delayMicroseconds( 1 );
            if ( counter == 255 )
            {
                break;
            }
        }
        laststate = digitalRead( DHT_PIN );
        if ( counter == 255 )
            break;
        /* ignore first 3 transitions */
        if ( (i >= 4) && (i % 2 == 0) )
        {
            /* shove each bit into the storage bytes */
            data[j / 8] <<= 1;
            if ( counter > 50 )
                data[j / 8] |= 1;
            j++;
        }
    }
    if ( (j >= 40) &&
         (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF) ) )
    {
        h = (float)((data[0] << 8) + data[1]) / 10;
        if ( h > 100 )
        {
            h = data[0];	// for DHT11
        }
        c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
        if ( c > 125 )
        {
            c = data[2];	// for DHT11
        }
        if ( data[2] & 0x80 )
        {
            c = -c;
        }
        f = c * 1.8f + 32;
        printf( "Humidity = %.1f %% Temperature = %.1f *C (%.1f *F)\n", h, c, f );
    }else  {
        printf( "Data not good, skip\n" );
    }
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    wiringPiSetupGpio();
    read_dht_data();
    ui->setupUi(this);
    ui->nhietDo->display(c);
    ui->doAm->display(h);
}
MainWindow::~MainWindow()
{
    delete ui;
}



