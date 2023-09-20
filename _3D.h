#pragma once
#include"EGE/graphics.h"
#include<algorithm>
#include<utility>
#include<array>
#include<cmath>
#include<list>
#define SHOW_CONSOLE 0
inline double sind(const double &a){ return sin(0.017453292519943295*a); }
inline double cosd(const double &a){ return cos(0.017453292519943295*a); }
class point3d{
public:
	double x,y,z,x2,y2;
	point3d &move(const double &tx,const double &ty,const double &tz){
		x+=tx,y+=ty,z+=tz;
		return *this;
	}
	point3d &scale(const point3d &c,const double &sx,const double &sy,const double &sz){
		x=(x-c.x)*sx+c.x;
		y=(y-c.y)*sy+c.y;
		z=(z-c.z)*sz+c.z;
		return *this;
	}
	point3d &rotate(const point3d &c,const double &dx,const double &dy,const double &dz);
	point3d &project(const int &focal){
		const double c=focal/(z?z:1);
		x2=x*c,y2=y*c;
		return *this;
	}
	point3d center()const{ return *this; }
};
template<typename objT,typename ctrT=std::list<objT>>
class contnr3d:public ctrT{
public:
	double depth=0;
	contnr3d(){}
	contnr3d(const ctrT &objs_):ctrT(objs_){}
	contnr3d &move(const double &tx,const double &ty,const double &tz){
		for(auto &p:*this) p.move(tx,ty,tz);
		return *this;
	}
	contnr3d &scale(const point3d &c,const double &sx,const double &sy,const double &sz){
		for(auto &p:*this) p.scale(c,sx,sy,sz);
		return *this;
	}
	contnr3d &rotate(const point3d &c,const double &dx,const double &dy,const double &dz){
		for(auto &p:*this) p.rotate(c,dx,dy,dz);
		return *this;
	}
	contnr3d &project(const int &focal){
		for(auto &p:*this) p.project(focal);
		return *this;
	}
	point3d center()const{
		point3d c(0,0,0);
		for(const auto &p:*this){
			auto b=p.center();
			c.move(b.x,b.y,b.z);
		}
		const int l=this->size();
		return {c.x/l,c.y/l,c.z/l};
	}
	contnr3d &gendepth(){
		auto c=center();
		depth=sqrt(c.x*c.x+c.y*c.y+c.z*c.z);
		return *this;
	}
};
class face3d:public contnr3d<point3d>{
public:
	color_t color=0;
	face3d(){}
	face3d(const std::list<point3d> &faces,const color_t &color_):contnr3d(faces),color(color_){}
	void render(const int &focal,const int &cx,const int &cy,const PIMAGE &img)const;
};
class render3d:public std::list<face3d*>{
public:
	render3d &project(const int &focal){
		for(auto &p:*this) p->project(focal);
		return *this;
	}
	render3d &gendepth(){
		for(auto &p:*this) p->gendepth();
		return *this;
	}
	render3d &sort_faces(){
		sort([](const face3d *const &a,const face3d *const &b){return a->depth>b->depth;});
		return *this;
	}
	void render(const int &focal,const int &cx,const int &cy,const PIMAGE &img)const{
		for(const auto &p:*this) p->render(focal,cx,cy,img);
	}
};
class rect3d:public contnr3d<face3d,std::array<face3d,6>>{
public:
	rect3d(){}
	rect3d(const point3d &a,const point3d &b,const color_t(&colors)[6]);
	std::pair<point3d,point3d> collisionbox()const;
};
class render3d_guard{
public:
	render3d *rd;
	std::list<std::list<face3d*>::iterator> fp;
	bool face_removed=0;
	template<typename ctrT>
	render3d_guard(contnr3d<face3d,ctrT> &ctr,render3d *const&rd_):rd(rd_){
		for(int i=0;i<6;i++) rd->push_back(&ctr[i]),fp.emplace_back(prev(rd->end()));
	}
	~render3d_guard(){
		if(!face_removed)
			for(auto &p:fp) rd->erase(p);
	}
	void remove(){
		if(!face_removed){
			face_removed=1;
			for(auto &p:fp) rd->erase(p);
			fp.erase(fp.begin(),fp.end());
		}
	}
};
byte colliding(const std::pair<point3d,point3d> &r1,const std::pair<point3d,point3d> &r2);
