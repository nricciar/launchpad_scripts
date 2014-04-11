double RTgoertzelFilter(int sample, double freq) {
    static double s_prev = 0.0;
    static double s_prev2 = 0.0;  
    static double totalpower = 0.0;  
    static int N = 0;
    double coeff,normalizedfreq,power,s;
    normalizedfreq = freq / SAMPLEFREQUENCY;
    coeff = 2*cos(2*M_PI*normalizedfreq);
    s = sample + coeff * s_prev - s_prev2;
    s_prev2 = s_prev;
    s_prev = s;
    N++;
    power = s_prev2*s_prev2+s_prev*s_prev-coeff*s_prev*s_prev2;
    totalpower += sample*sample;
    if (totalpower == 0) totalpower=1;
    return power / totalpower / N;
}
double tandemRTgoertzelFilter(int sample, double freq) {
    static double s_prev[2] = {0.0,0.0};
    static double s_prev2[2] = {0.0,0.0};  
    static double totalpower[2] = {0.0,0.0};  
    static int N=0;       
    double coeff,normalizedfreq,power,s;
    int active;
    static int n[2] = {0,0};
    normalizedfreq = freq / SAMPLEFREQUENCY;
    coeff = 2*cos(2*M_PI*normalizedfreq);
    s = sample + coeff * s_prev[0] - s_prev2[0];
    s_prev2[0] = s_prev[0];
    s_prev[0] = s;
    n[0]++;
    s = sample + coeff * s_prev[1] - s_prev2[1];
    s_prev2[1] = s_prev[1];
    s_prev[1] = s;    
    n[1]++;
    N++;


    active = (N / RESETSAMPLES) & 0x01;
    if (1 >= RESETSAMPLES) { // reset inactive
        s_prev[1-active] = 0.0;
        s_prev2[1-active] = 0.0;  
        totalpower[1-active] = 0.0;  
        n[1-active]=0;    
    };
    totalpower[0] += sample*sample;
    totalpower[1] += sample*sample;
    power = s_prev2[active]*s_prev2[active]+s_prev[active]
       * s_prev[active]-coeff*s_prev[active]*s_prev2[active];
    return power / (totalpower[active]+1e-7) / n[active];
}
