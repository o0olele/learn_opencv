@[TOC](目录)
# 说在前面
> - opencv版本：4.0.1
> - 操作系统：win10
> - vs版本：2017
> - 官方文档：[How to scan images, lookup tables and time measurement with OpenCV](https://docs.opencv.org/4.0.1/db/da5/tutorial_how_to_scan_images.html)
> - 其他说明：自学，记录，[demo](https://github.com/o0olele/learn_opencv/tree/master/demo2)

# Color Space Reduction
- 对于c/c++，使用unsigned char（8 bits）来存储的话，每个像素的单个通道就有256个值，这给算法性能带来了沉重的负担；但是，大部分时候并不需要使用这全部的256个值。

- 所以我们引入了 <font color="#ff0000">__color space reduction__ </font> 这个东东：
$$ I_{new} = ({I_{old}\over 10 }) * 10 $$
我们将每个像素的每个通道的值均使用上述公式处理一遍，这样就将256个值缩减至26个值。
例如：5 → 0    255 → 25

0-9    | 10-19 | 20-29 | 30-39 | 40-49 | 50-59 | ... |
------- | ------- | ------ | ------ | ------ | ------ | ------
0 | 10 | 20 | 30 | 40 | 50 | ...

- 但是上述的方法使用了除法和乘法，并不适合在**遍历像素**的时候对每个像素进行这种操作（乘除计算比较慢，相对于加、减、移位）

- 所以我们又引入了另外一种东东，<font color="#ff0000">__lookup table__ </font>；这是个一维或者多维的数组，数组的值是事先已经计算好的。如下表：

0  | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | ...
-- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | --
0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | ...

- 使用lookup table，我们就不需要进行计算了，只需取出table中的值即可，例如254，使用table[254]即可。

# cv::getTickCount() & cv::getTickFrequency()
- 用于计时
~~~c++
double t = (double)getTickCount();//当前时钟周期数

// do something ...

//getTickFrequency() 获取每秒时钟周期数
t = ((double)getTickCount() - t)/getTickFrequency();
cout << "Times passed in seconds: " << t << endl;
~~~

# 遍历像素
- ## The efficient way 

     + 使用指针进行访问(operator []) 
    ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190625222850865.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMzNDQ2MTAw,size_16,color_FFFFFF,t_70)
    + 如上图，Mat类的方法 **ptr(i)** 返回**第 i 行的首地址**，对于三通道的Mat，每一行的数据是连续的，即可以使用循环自增的方式访问每一个数据（channel*cols， 每个数据8bit）
    + 这里还是存在一个问题，那就是 **行与行之间的地址可能<font color="#ff0000">不是</font>连续的** ，即第 i 行尾地址为 n ，第 i+1 行首地址可能不是 n+1，这是由于mat类在创建的时候可以使用 **Mat::col(取列)** 以及**Mat::diag(取对角线)**；下面是col()与diag()函数的测试demo。
~~~c++
//code of "col() & diag() demo"

    Mat A = (Mat_<double>(3, 3) << 1, 2, 3, 4, 5, 6, 7, 8, 9);
	Mat c1 = A.col(0);
	Mat c2 = A.diag(0);

	cout << "A      continuous? " << A.isContinuous() << endl;
	cout << "A \n" << A << endl;
	cout << "col()  continuous? " << c1.isContinuous() << endl;
	cout << "col(0) \n" << c1 << endl;
	cout << "diag() continuous? " << c2.isContinuous() << endl;
	cout << "diag(0) \n" << c2 << endl;
~~~  
![result of the demo](https://img-blog.csdnimg.cn/20190626112816457.png#pic_center?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMzNDQ2MTAw,size_16,color_FFFFFF,t_70)

~~~c++
// Code of "The efficient way"

Mat& ScanImageAndReduceC(Mat& I, const uchar* const table)
{
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);

	int channels = I.channels();

	int nRows = I.rows;//行数
	int nCols = I.cols * channels;//列数

	if (I.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}

	int i, j;
	uchar* p;
	for (i = 0; i < nRows; ++i)
	{
		p = I.ptr<uchar>(i);
		for (j = 0; j < nCols; ++j)
		{
			p[j] = table[p[j]];
		}
	}
	return I;
}
~~~

- ## The iterator (safe) method 
   + 使用迭代器，确定start & end，有效**防止数组越界**，并且**不需要考虑是否连续**。
~~~c++
Mat& ScanImageAndReduceIterator(Mat& I, const uchar* const table)
{
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);

	const int channels = I.channels();
	switch (channels)
	{
	case 1:
	{
		MatIterator_<uchar> it, end;
		for (it = I.begin<uchar>(), end = I.end<uchar>(); it != end; ++it)
			*it = table[*it];
		break;
	}
	case 3:
	{
		MatIterator_<Vec3b> it, end;
		for (it = I.begin<Vec3b>(), end = I.end<Vec3b>(); it != end; ++it)
		{
			(*it)[0] = table[(*it)[0]];
			(*it)[1] = table[(*it)[1]];
			(*it)[2] = table[(*it)[2]];
		}
	}
	}

	return I;
}
~~~
- ## On-the-fly address calculation with reference returning 
   + **不推荐用来遍历像素**
   + 一般是用来访问指定位置的像素的
   + 基本流程：从第一行开始，遍历第一行的所有像素；然后第二行、第三行
~~~c++
Mat& ScanImageAndReduceRandomAccess(Mat& I, const uchar* const table)
{
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);

	const int channels = I.channels();
	switch (channels)
	{
	case 1:
	{
		for (int i = 0; i < I.rows; ++i)
			for (int j = 0; j < I.cols; ++j)
				I.at<uchar>(i, j) = table[I.at<uchar>(i, j)];
		break;
	}
	case 3:
	{
		Mat_<Vec3b> _I = I;

		for (int i = 0; i < I.rows; ++i)
			for (int j = 0; j < I.cols; ++j)
			{
				_I(i, j)[0] = table[_I(i, j)[0]];
				_I(i, j)[1] = table[_I(i, j)[1]];
				_I(i, j)[2] = table[_I(i, j)[2]];
			}
		I = _I;
		break;
	}
	}

	return I;
}
~~~
- ## The Core Function 
  + 使用了**多线程**，所以快。
~~~c++
Mat lookUpTable(1, 256, CV_8U);
uchar* p = lookUpTable.ptr();
for( int i = 0; i < 256; ++i)
    p[i] = table[i];
//......
LUT(I, lookUpTable, J);
~~~
- ## 测试结果对比(release)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190625235342719.png) 
- ## 总结
  + 根据上面的测试结果，LUT方法最快，并且官方也推荐使用；
  + 如果你更喜欢自己写的话，推荐使用迭代器（安全，但效率较低）
  
  
- ## 其他
（测试的时候遇到个奇怪的问题）
~~~c++
    Mat Mat_inA = imread("test.jpg");//上面测试结果对应这种
	Mat Mat_inB = imread("test.jpg");
	Mat Mat_inC = imread("test.jpg");
	Mat I = imread("test.jpg");
~~~

~~~c++
    Mat Mat_inA = imread("test.jpg");//使用这种测试结果不一样
	Mat Mat_inB = Mat_inA.clone();
	Mat Mat_inC = Mat_inA.clone();
	Mat I = Mat_inA.clone();
~~~

~~~c++
J = ScanImageAndReduceC(Mat_inA, table);
//......
J = ScanImageAndReduceIterator(Mat_inB, table);
//......
J = ScanImageAndReduceRandomAccess(Mat_inC, table);
//......
LUT(I, lookUpTable, J);
~~~
上面两种方式得到的测试结果不一样，不知道为啥

***
END-2019.6.26
