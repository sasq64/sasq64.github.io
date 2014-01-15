#include <memory>
#include <functional>
#include <vector>
#include <cstdint>

using namespace std;

struct bitmap {
    bitmap(int w, int h) :
        w(w), h(h), pixels(make_shared<vector<uint8_t>>(w*h)) {
    }

    uint8_t& operator[](const int &i) { 
        return (*pixels)[i];
    }

    const int w;
    const int h;
private:
    shared_ptr<vector<uint8_t>> pixels;
};

void fill(bitmap bm, uint32_t color) {
    for(int i=0; i<bm.w*bm.h; i++)
        bm[i] = color;
}

function<bitmap()> fractalGenerator(int w, int h) {
    bitmap bm(w,h);
    return [=]() mutable -> bitmap {
        for(int y=0; y<h; y++)
            for(int x=0; x<w; x++)
                bm[x+y*w] = x*y;
        return bm;
    };
}

int main() {
	bitmap myBitmap(100,100);
	fill(myBitmap, 0x33);

	auto f = fractalGenerator(10, 10);
	auto bm2 = f();
}

