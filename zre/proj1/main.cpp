#include <iostream>
#include <cstdint>
#include <algorithm>
#include "armadillo"

// struct for wav header
struct wav_header {
    char riff_tag[4];
    int32_t riff_length;
    char wave_tag[4];
    char fmt_tag[4];
    int32_t fmt_length;
    int16_t audio_format;
    int16_t num_channels;
    int32_t sample_rate;
    int32_t byte_rate;
    int16_t block_align;
    int16_t bits_per_sample;
    char data_tag[4];
    int32_t data_length;
};

// write data to wav file
bool wavwrite(arma::vec data, const char * file) {
    int samples_per_second = 8000;
    int16_t bits_per_sample = 16;
    int data_length = data.n_rows*bits_per_sample/8;
    wav_header header = {
        {'R','I','F','F'},
        data_length+36,
        {'W','A','V','E'},
        {'f','m','t',' '},
        16,
        1,
        1,
        samples_per_second,
        samples_per_second*(bits_per_sample/8),
        2,
        bits_per_sample,
        {'d','a','t','a'},
        data_length,
    };
    
    std::ofstream out(file, std::ios::binary);
    if (!out.is_open())
    {
        std::cerr << "Error: Opening output file" << std::endl;
        return false;
    }
    
    out.write((char *)&header,sizeof(header));
    for(int i=0;i<data.n_rows;++i){
        int16_t sample = static_cast<int16_t>(data(i)*32767.0);
        out.write((char *)&sample,sizeof(sample));
    }
    out.close();
    return true;
}

arma::vec filter(double b, const arma::vec &a, const arma::vec &x, arma::vec &z) {
    arma::vec y(x.size());
    for (int i = 0; i < x.size(); ++i) {
        y(i) = (b * x(i) + z(0));
        int j;
        for (j = 1; j < z.size(); ++j) {
            z(j - 1) = z(j) - a(j) * y(i);
        }
        z(j - 1) = -a(j) * y(i);
    }
    return y;
}

arma::vec synthesize(arma::mat A, arma::rowvec G, arma::uvec L, int P, int lram) {
    int Nram = G.n_cols;
    arma::vec init = arma::zeros(P);
    arma::vec ss = arma::zeros(Nram * lram);
    arma::vec excit;

    int nextvoiced = 0;
    int from = 0, to = from + lram - 1;
    for (int n = 0; n < Nram; ++n) {
        arma::vec a = arma::join_vert(arma::ones(1), A.col(n));
        double g = G(n);
        int l = L(n);

        if (l == 0)
            excit = arma::randn(lram);
        else {
            int m = (lram - nextvoiced) / l + 1;
            arma::uvec where(m);
            for (int i = 0; i < m; ++i) {
                where(i) = nextvoiced;
                nextvoiced += l;
            }
            nextvoiced -= lram;
            excit = arma::zeros(lram);
            excit.each_row(where) = arma::ones(1);
        }
        double power = sum(square(excit)) / lram;
        excit = excit / sqrt(power);
        arma::vec synt = filter(g, a, excit, init);
        ss.rows(from, to) = synt;
        from += lram;
        to = from + lram - 1;
    }
    return ss;
}

bool decoder(char ** argv) {
    arma::mat cb, gcb, Adec;
    arma::rowvec Gdec;
    arma::umat agL;
    if(!cb.load(argv[1])){
        std::cerr << "Error: Cant load lpc data" << std::endl;
        return false;
    }
    if(!gcb.load(argv[2])){
        std::cerr << "Error: Cant load gain data" << std::endl;
        return false;
    }
    if(!agL.load(argv[3])){
        std::cerr << "Error: Cant load cod data" << std::endl;
        return false;
    }
    agL.cols(0, 1).each_col() -= arma::ones<arma::uvec>(agL.n_rows);
    Adec = cb.cols(agL.col(0));
    Gdec = gcb.cols(agL.col(1));
    arma::vec ss = synthesize(Adec, Gdec, agL.col(2), 10, 160);

    return wavwrite(ss,argv[4]);
}

int main(int argc, char ** argv) {
    if(argc != 5){
        std::cerr << "Usage:" << std::endl
                  << "zre_proj1_linux cb_lpc.txt cb_gain.txt in.cod out.wav" << std::endl;
        return 1;
    }
    decoder(argv);
    return 0;
}
