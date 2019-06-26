;
#include <lcd.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define BAR_COUNT 20
#define BAR_COLOR RED
#define BAR_BACKGROUND BLACK

int bar_width = 4;

void init_bars()
{
    lcd_clear(BLACK);
    bar_width = LCD_Y_MAX / BAR_COUNT;
}

int last_value[BAR_COUNT];
void update_height(int index, int value)
{
    // printf("update height %d = %d\n",index,value);
    if(last_value[index] == value)
        return;
    if(last_value[index] > value)
    {
        lcd_fill_rectangle(value, index * bar_width, last_value[index] + 1, index * bar_width + bar_width - 2, BAR_BACKGROUND);
    }
    else
    {
        int v = last_value[index];
        if(v != 0)
        {
            v--;
        }
        lcd_fill_rectangle(v, index * bar_width, value, index * bar_width + bar_width - 2, BAR_COLOR);
    }
    last_value[index] = value;
}

double log_place(int index, int count)
{
    return log10(((float)index / count) * 9 + 1) * BAR_COUNT;
}

void update_bars(float *power, float max_power, int count)
{
    float last_mean = 0;
    int bar_num = 0;

    float sum = 0.0f;
    int sum_count = 0;
    for(int i = 0; i < count; i++)
    {
        float place = log_place(i, count);
        int next_place = log_place(i + 1, count);
        double index;
        float mean = modf(place, &index);
        // printf("%03d/%03d (%3f): place= %f\n",i,count,((float)i/count),place);
        if(index == next_place)
        {
            sum += power[(int)index];
            sum_count++;
        }
        else
        {
            sum += power[(int)index] * mean;
            sum_count++;

            int height = (sum / sum_count / max_power) * LCD_X_MAX;

            update_height(bar_num, height);

            sum = power[(int)index] * (1 - mean);
            sum_count = 0;
            bar_num++;
        }
    }
}
