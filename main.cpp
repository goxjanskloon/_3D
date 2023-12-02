#include"_3D.h"
#include<random>
inline bool keydown(int vKey){ return GetAsyncKeyState(vKey)&0x8000; }
inline int genint(const int &l,const int &r){
    std::random_device rd;
    std::mt19937 gen(rd());
    return std::uniform_int_distribution<>(l,r)(gen);
}
class render:public render3d{
public:
    int focal=400,xl=1000,yl=600,cx=xl>>1,cy=yl>>1;
    PIMAGE img=newimage(xl,yl);
    render &init(){
        initgraph(xl,yl,getinitmode());
        ege_enable_aa(1,img);
        setbkmode(TRANSPARENT,img);
        setfont(20,0,"Consolas",img);
        setcolor(WHITE,img);
        return *this;
    }
    render &flush(){
        gendepth().sort_faces().project(focal).render(focal,cx,cy,img);
        cleardevice();
        putimage(0,0,img);
        cleardevice(img);
        return *this;
    }
};
template<typename... argT>
inline void xyprintf(const int &x,const int &y,const PIMAGE &img,const char *fmt,argT... args){
    char bf[8192];
    sprintf(bf,fmt,args...);
    outtextxy(x,y,bf,img);
}
void test(){
    rect3d rect1({-50,50,450},{50,-50,550},{color_t(BLUE),color_t(RED),color_t(YELLOW),color_t(GREEN),color_t(BROWN),color_t(CYAN)}),rect1t=rect1,
          rect2({-50,350,450},{50,450,550},{color_t(BLUE),color_t(RED),color_t(YELLOW),color_t(GREEN),color_t(BROWN),color_t(CYAN)}),rect2t=rect2,r1,r2;
    render rd;
    render3d_guard rg1(rect1,&rd),rg2(rect2,&rd),rgr1(r1,&rd),rgr2(r2,&rd);
    rd.init();
    auto gendeg=[](){return genint(0,359);};
    auto c1=rect1.center();
    for(double dx=0,dy=0;;rect1=rect1t,rect2=rect2t){
        rect1.rotate({0,0,500},gendeg(),gendeg(),gendeg());
        rect2.rotate({0,0,500},gendeg(),gendeg(),gendeg());
        auto c2=rect2.center();
        for(double tx=(c1.x-c2.x)/60,ty=(c1.y-c2.y)/60,tz=(c1.z-c2.z)/60,t=1;t<=120;t++,Sleep(5)){
            int mx,my;
            mousepos(&mx,&my);
            dx=(rd.cy-my)*180.0/rd.cy,dy=(rd.cx-mx)*180.0/rd.cx;
            rect2.move(tx,ty,tz);
            auto pb1=rect1.collisionbox(),pb2=rect2.collisionbox();
            if(keydown(27)) return;
            if(keydown('C')){
                r1=rect3d(pb1.first,pb1.second,{EGECOLORA(BLUE,100),EGECOLORA(RED,100),EGECOLORA(YELLOW,100),EGECOLORA(GREEN,100),EGECOLORA(BROWN,100),EGECOLORA(CYAN,100)});
                r2=rect3d(pb2.first,pb2.second,{EGECOLORA(BLUE,100),EGECOLORA(RED,100),EGECOLORA(YELLOW,100),EGECOLORA(GREEN,100),EGECOLORA(BROWN,100),EGECOLORA(CYAN,100)});
            }
            else{
                r1=rect3d(pb1.first,pb1.second,{EGECOLORA(BLUE,0),EGECOLORA(RED,0),EGECOLORA(YELLOW,0),EGECOLORA(GREEN,0),EGECOLORA(BROWN,0),EGECOLORA(CYAN,0)});
                r2=rect3d(pb2.first,pb2.second,{EGECOLORA(BLUE,0),EGECOLORA(RED,0),EGECOLORA(YELLOW,0),EGECOLORA(GREEN,0),EGECOLORA(BROWN,0),EGECOLORA(CYAN,0)});
            }
            r1.rotate(c1,dx,dy,0),r2.rotate(c1,dx,dy,0);
            auto t1=rect1,t2=rect2;
            rect1.rotate(c1,dx,dy,0),rect2.rotate(c1,dx,dy,0);
            byte c=colliding(pb1,pb2);
            if(c>>3) outtextxy(0,0,"colliding",rd.img);
            xyprintf(0,20,rd.img,"X:%d Y:%d Z:%d",c>>2&1,c>>1&1,c>>0&1);
            xyprintf(0,40,rd.img,"dx=%f dy=%f",dx,dy);
            rd.flush();
            rect1=t1,rect2=t2;
        }
    }
}
int main(){
    test();
    return 0;
}
