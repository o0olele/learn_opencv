@[TOC](目录)
# 说在前面
> - opencv版本：4.0.1
> - 操作系统：win10
> - vs版本：2017
> - 官方文档：[Mat - The Basic Image Container](https://docs.opencv.org/4.0.1/d6/d6d/tutorial_mat_the_basic_image_container.html)
> - 其他说明：自学，记录

# Mat创建
- 我们使用一个简单的demo来看一下（复制构造函数、运算符重载）和（copyTo、clone函数两者之间的区别）
~~~c++
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	Mat A, C;                          // creates just the header parts
	A = imread("feng.png", IMREAD_COLOR); 
	// here we'll know the method used (allocate matrix)

	Mat B(A);                                 
	// Use the copy constructor
	//使用复制构造函数

	C = A;                                    
	// Assignment operator

	Mat F = A.clone();
	Mat G;
	A.copyTo(G);

	cout << "A ptr: " << hex << (void *)A.data << endl;
	cout << "B ptr: " << hex << (void *)B.data << endl;
	cout << "C ptr: " << hex << (void *)C.data << endl;
	cout << "F ptr: " << hex << (void *)F.data << endl;
	cout << "G ptr: " << hex << (void *)G.data << endl;
	return 0;
}


~~~
- ___运行结果___
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190625164622958.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMzNDQ2MTAw,size_16,color_FFFFFF,t_70)
可以看到A、B、C三者的<font color="#ff0000">**data起始地址是相同**</font>的，
而F、G与A、B、C不同，F与G也是不同的。
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190625171532911.png#pic_center)
# 相关源码
~~（源码好难分析啊，暂时只找到这俩）~~
([copy.cpp 地址](https://github.com/opencv/opencv/blob/master/modules/core/src/copy.cpp))

~~~ c++

void Mat::copyTo( OutputArray _dst ) const

Mat& Mat::operator = (const Scalar& s)

~~~
