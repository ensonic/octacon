// display layout code

#ifndef UI_H
#define UI_H

#include <U8g2lib.h>

struct UIParam {
    String name, prettyvalue;
    unsigned value;
};

class UI {
public:
    UI(U8G2 *d1, U8G2 *d2 = nullptr);

    // initialize displays
    void init();
    // redraw for given parameter index
    void draw(unsigned ix); 

    void setName(unsigned ix, char *str, unsigned len);
    void setPrettyValue(unsigned ix, char *str, unsigned len);
    void setValue(unsigned ix, unsigned value);

private:
    U8G2 *d1, *d2;
    static const unsigned numParams = 8;
    UIParam p[numParams] = { {"","",0}, };
    
    void initPage(U8G2 *d);
    void drawPage(U8G2 *d, UIParam p0, UIParam p1, UIParam p2, UIParam p3);
    void drawColumn(U8G2 *d, unsigned x, UIParam p0, UIParam p1);
};

#endif // UI_H