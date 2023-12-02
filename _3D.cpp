#include"_3D.h"
#include<vector>
#include<cfloat>
point3d &point3d::rotate(const point3d &c,const double &dx,const double &dy,const double &dz){
	x-=c.x,y-=c.y,z-=c.z;
	const double sindx=sind(dx),sindy=sind(dy),sindz=sind(dz),cosdx=cosd(dx),cosdy=cosd(dy),cosdz=cosd(dz);
	auto s=*this;
	y=y*cosdx-z*sindx;
	z=s.y*sindx+z*cosdx;
	x=z*sindy-x*cosdy;
	z=z*cosdy+s.x*sindy;
	s.x=x;
	x=x*cosdz-y*sindz;
	y=s.x*sindz+y*cosdz;
	x+=c.x,y+=c.y,z+=c.z;
	return *this;
}
void face3d::render(const int &focal,const int &cx,const int &cy,const PIMAGE &img)const{
	std::vector<ege_point> pnts;
	std::list<point3d> vtc=*this;
	for(auto p=vtc.begin();p!=vtc.end();p++)
		if(p->z<0) vtc.insert(p,*p);
	for(auto p=vtc.begin();p!=vtc.end();p++){
		auto pp=prev(p==vtc.begin()?vtc.end():p),pn=p==prev(vtc.end())?vtc.begin():next(p);
		if(p->z>=0||pp->z>=0||pn->z>=0){
			point3d c;
			if(p->z>=0) c=*p;
			else{
				if(pp->z>=0){
					const double k=pp->z/(pp->z-p->z);
					c={pp->x-k*(pp->x-p->x),pp->y-k*(pp->y-p->y),0};
				}
				else{
					const double k=pn->z/(pn->z-p->z);
					c={pn->x-k*(pn->x-p->x),pn->y-k*(pn->y-p->y),0};
				}
				c.project(focal);
			}
			pnts.emplace_back(ege_point{float(cx+c.x2),float(cy-c.y2)});
		}
	}
	setfillcolor(color,img);
	setlinestyle(NULL_LINE,0,0,img);
	ege_fillpoly(pnts.size(),pnts.data(),img);
}
rect3d::rect3d(const point3d &a,const point3d &b,const color_t(&colors)[6]):contnr3d({
	face3d({{a.x,a.y,a.z},{b.x,a.y,a.z},{b.x,b.y,a.z},{a.x,b.y,a.z}},colors[0]),
	face3d({{a.x,a.y,b.z},{b.x,a.y,b.z},{b.x,b.y,b.z},{a.x,b.y,b.z}},colors[1]),
	face3d({{a.x,a.y,b.z},{a.x,a.y,a.z},{a.x,b.y,a.z},{a.x,b.y,b.z}},colors[2]),
	face3d({{b.x,a.y,a.z},{b.x,a.y,b.z},{b.x,b.y,b.z},{b.x,b.y,a.z}},colors[3]),
	face3d({{a.x,a.y,b.z},{b.x,a.y,b.z},{b.x,a.y,a.z},{a.x,a.y,a.z}},colors[4]),
	face3d({{a.x,b.y,b.z},{b.x,b.y,b.z},{b.x,b.y,a.z},{a.x,b.y,a.z}},colors[5])}){}
std::pair<point3d,point3d> rect3d::collisionbox()const{
	point3d p1{DBL_MAX,-DBL_MAX,DBL_MAX},p2{-DBL_MAX,DBL_MAX,-DBL_MAX};
	for(const auto &f:*this)
		for(const auto &p:f) p1.x=std::min(p1.x,p.x),p1.y=std::max(p1.y,p.y),p1.z=std::min(p1.z,p.z),p2.x=std::max(p2.x,p.x),p2.y=std::min(p2.y,p.y),p2.z=std::max(p2.z,p.z);
	return std::make_pair(p1,p2);
}
byte colliding(const std::pair<point3d,point3d> &r1,const std::pair<point3d,point3d> &r2){
	const point3d &p11=r1.first,&p12=r1.second,&p21=r2.first,&p22=r2.second;
	byte c=((p21.x<p11.x&&p11.x<p22.x||p21.x<p12.x&&p12.x<p22.x||p11.x<p21.x&&p21.x<p12.x||p11.x<p22.x&&p22.x<p12.x)<<2
		   |(p22.y<p11.y&&p11.y<p21.y||p22.y<p12.y&&p12.y<p21.y||p12.y<p21.y&&p21.y<p11.y||p12.y<p22.y&&p22.y<p11.y)<<1
		   |(p21.z<p11.z&&p11.z<p22.z||p21.z<p12.z&&p12.z<p22.z||p11.z<p21.z&&p21.z<p12.z||p11.z<p22.z&&p22.z<p12.z)<<0);
	return c|((c>>2&1)&(c>>1&1)&(c&1))<<3;
}
