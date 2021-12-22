class MyMath
{
public:
	MyMath(void){};
	~MyMath(void){};

	// 解一元二次方程axx+bx+c=0 考虑各种情况
	// -2:有无穷多个根 -1:无根 0:两个虚根 1:1根 2:2根
	// 	cv::solvePoly(coef, roots, 1);//  解x2−2x+1=0时出现错误,必须设置maxIters=1才正确
	static int mysolvefunction(double a, double b, double c, double &x1, double &x2)
	{
		if(a==0 && b==0)
			return c==0 ? -2 : -1;

		double d=b*b-4*a*c;
		if(d<0)
		{
			x1=-b/(2*a);
			x2=sqrt(-d)/(2*a);
			//cout<<x1<<"+"<<fabs(x2)<<"i"<<endl;	// 判别式<0 两个虚根
			//cout<<x1<<"-"<<fabs(x2)<<"i"<<endl;
			return 0;
		}
		
		if(a==0&&b!=0)
		{
			x1=-c/b;
			return 1;								// 二次项为0, 只有一根
		}

		if(d==0)
		{
			x1=(-b)/(2*a);
			return 1;								// 判别式=0 两个相同根
		}
		
		if(d>0)
		{
			double e=sqrt(d);
			x1=(-b+e)/(2*a);
			x2=(-b-e)/(2*a);
			if(x1 < x2)
			{
				double t=x1;
				x1=x2;
				x2=x1;
			}
			return 2;								// 判别式>0 两个不同根
		}
	}

	// 解一元二次方程 形如:(ax+b)(cx+d)=(ex+f)(gx+h)
	static int mysolvefunction( double a, double b, double c, double d,
								double e, double f, double g, double h,
								double &x1, double &x2)
	{
		return mysolvefunction(a*c-e*g, a*d+b*c-e*h-f*g, b*d-f*h, x1, x2);
	}

	// 简化版 只取一个较大根
	static double mysolvefunction(double a, double b, double c, double d,
								  double e, double f, double g, double h)
	{
		double x1, x2;
		double ret = mysolvefunction(a*c-e*g, a*d+b*c-e*h-f*g, b*d-f*h, x1, x2);
		return ret > 0 ? x1 : INT_MIN;
	}
};
