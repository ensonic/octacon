// display layout code

#ifndef SRC_UI_H
#define SRC_UI_H

#include <U8g2lib.h>
#include <config.h>

#ifdef CTRL_HIRES
// scaling for ui sliders
#define UI_SCALE 128
// raw value digits
#define UI_DIGITS 5
#else
#define UI_SCALE 1
#define UI_DIGITS 3
#endif


struct UIParam {
    String name, prettyvalue;
    unsigned value;
};

class UI {
public:
    UI(U8G2 *d1, U8G2 *d2 = nullptr);

    // initialize displays
    void init();
    void begin();
    // redraw for given parameter index
    void draw(unsigned ix); 

    void setName(unsigned ix, char *str, unsigned len);
    void setPrettyValue(unsigned ix, char *str, unsigned len);
    void setValue(unsigned ix, unsigned value);
    void enableExtInfo(bool enable);
    void setInfo(char *str);

private:
    U8G2 *d1, *d2;
    static const unsigned numParams = 8;
    bool extInfo = false;
    UIParam p[numParams] = { {"","",0}, };
    char info1[50],info2[50]; // info text in the middle ~ 26 chars per display
    
    void initPage(U8G2 *d);
    void drawPage(U8G2 *d, UIParam &p0, UIParam &p1, UIParam &p2, UIParam &p3, char *str);
    void drawColumn(U8G2 *d, unsigned x, UIParam &p0, UIParam &p1);
};

#endif // SRC_UI_H