// display layout code

#include <debug.h>
#include <ui.h>

extern Debug dbg;

UI::UI(U8G2 *d1, U8G2 *d2) : d1(d1), d2(d2) {}

void UI::init(void) {
    initPage(d1);
    initPage(d2);

    // TODO: make this an "about" page and also show version?
    // we could report via sysex, but also trigger by holding buttons
    // If we feel crazy, we can also embed a pong game :)
    const char *logo = "OCTACON";
    auto w = d1->getStrWidth(logo);
    auto x = 64 - w; // we print with double size

    d1->clearBuffer();
    d1->drawStrX2(x,32+8, logo);
    d1->updateDisplay(); // display refresh: 3043 µs
    if(d2) {
        d2->clearBuffer();
        d2->drawStrX2(x,32+8, logo);
        d2->updateDisplay();
    }
    delay(100);
}

void UI::begin() {
    enableExtInfo(0);
}

void UI::draw(unsigned ix) {
    if (!extInfo) {
        for (unsigned i=0; i<numParams; i++) {
            p[i].prettyvalue = String(u8x8_u16toa(p[i].value, UI_DIGITS));
        }
    }
    if (!(ix & 0x2)) {
      this->drawPage(d1, p[0], p[1], p[4], p[5]);
    } else {
      this->drawPage(d2, p[2], p[3], p[6], p[7]);
    }
}

void UI::setName(unsigned ix, char *str, unsigned len) {
    if (ix >= numParams) {
        dbg.printf("ix=%u > %u\n", ix, numParams);
        return;
    }
    p[ix].name = String(str, len);
    draw(ix);
}

void UI::setPrettyValue(unsigned ix, char *str, unsigned len) {
    if (ix >= numParams) {
        dbg.printf("ix=%u > %u\n", ix, numParams);
        return;
    }
    p[ix].prettyvalue = String(str, len);
    draw(ix);
}

void UI::setValue(unsigned ix, unsigned value) {
    if (ix >= numParams) {
        dbg.printf("ix=%u > %u\n", ix, numParams);
        return;
    }
    p[ix].value = value / UI_SCALE;
    draw(ix);
}

void UI::enableExtInfo(bool enable) {
    extInfo = enable;
    if (!extInfo) {
        const String ixstr[] = {"0", "1", "2", "3", "4", "5", "6", "7"};
        for (unsigned i=0; i<numParams; i++) {
            p[i].name = String("Param ") + ixstr[i];
        }
    }
    draw(0);
    // draw(2);
}

// private impl

void UI::initPage(U8G2 *d) {
    if (!d) return;

    d->begin();
    // less flicker: 9.5 Timing & Driving Scheme Setting Command Table 
    d->sendF("ca", 0xd5, 0xF0); 
    d->setContrast(200);  //  Brightness setting from 0 to 255
    d->setFont(
    // 6 pixel
    // u8g2_font_spleen5x8_mf // same width as: u8g2_font_helvR08_tf
    // 7 pixel
    u8g2_font_NokiaSmallPlain_tf
    // 8 pixel
    // u8g2_font_helvR08_tf // not monospaced, can fit more text
    // u8g2_font_spleen6x12_mf // m=monospaced,f= full charset
    );
    d->setFontMode(1); // make transparent (no bg)
}

void UI::drawPage(U8G2 *d, UIParam p0, UIParam p1, UIParam p2, UIParam p3) {
    if (!d) return;

    d->clearBuffer();
    drawColumn(d, 0, p0, p2);
    drawColumn(d, 64, p1, p3);
    d->updateDisplay(); // display refresh: 3059 µs
    // TODO: if we update from the callback, we maybe send a bit mask for which value changed
    // https://github.com/olikraus/u8g2/wiki/u8g2reference#updatedisplayarea
    //unsigned ts0 = micros();
    //d->updateDisplayArea(0,0,8,4); // display refresh: 827 µs
    //ts0=micros()-ts0;
    //dbg.printf("refresh: %u µs\n", ts0);
}

void UI::drawColumn(U8G2 *d, unsigned x, UIParam p0, UIParam p1) {
    unsigned w=64;
    d->setDrawColor(1);
    d->setMaxClipWindow();
    // value box
    d->drawFrame(x, 0, 63, 15);
    d->drawBox(x, 1, p0.value/2, 14);
    d->drawFrame(x, 48, 63, 15);
    d->drawBox(x, 49, p1.value/2, 14);
    // param name
    d->setClipWindow(x, 16, x + w, 26);
    d->drawStr(x, 26, p0.name.c_str());  // 26 = 16 + 8 + 2
    d->setClipWindow(x, 35, x + w, 45);
    d->drawStr(x, 45, p1.name.c_str());
    // pretty name
    x++; w--;
    d->setDrawColor(2);
    d->setClipWindow(x, 2, x + w, 4+8);
    d->drawStr(x+1, 2+8, p0.prettyvalue.c_str());
    d->setClipWindow(x, 50, x + w, 52+8);
    d->drawStr(x+1,50+8, p1.prettyvalue.c_str());  
}
