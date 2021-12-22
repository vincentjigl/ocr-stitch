#ifndef MYQTOOL_H
#define MYQTOOL_H

//#include <QtWidgets>
//namespace cv {
//class Mat;
//}

// 添加 按钮
#define MY_ADDBTN(lname,method,layout)  btn = new QPushButton(QWidget::tr(lname));    \
                                                btn->setFixedWidth(50);                                 \
                                                connect(btn,&QPushButton::clicked,this,method); \
                                                layout->addWidget(btn)  // , 0, Qt::AlignRight)
// 添加 按钮
#define MY_ADDBTNP(lname,method,n,layout)  btn = new QPushButton(QWidget::tr(lname));    \
                                                btn->setFixedWidth(50);                                 \
                                                connect(btn,&QPushButton::clicked,this,[=](){method(n);}); \
                                                layout->addWidget(btn)  // , 0, Qt::AlignRight)

class MyQTool
{
public:
    MyQTool(){}

    // 添加 分割横线   垂直布局
    static void myAddHLine(QVBoxLayout *layout)
    {
        QFrame *line = new QFrame();
        //line->setGeometry(QRect(40, 180, 400, 30));   // 矩形直线
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        //line->raise();
        layout->addWidget(line);
    }

    // 添加 静态文字   水平布局
    static void myAddHLabel(const char* lname, QHBoxLayout *layout)
    {
        QLabel *lbl = new QLabel(QWidget::tr(lname));
        layout->addWidget(lbl);
//        layout->addStretch();
    }

    // 添加 静态文字  返回一个空文字控件,用于输出显示  水平布局
    static QLabel* myAddHLabel2(const char* lname, QHBoxLayout *layout)
    {
        const QFont qf("Consolas",12);
        QLabel *lbl = new QLabel(QWidget::tr(""));
        lbl->setFont(qf);
        layout->addWidget(new QLabel(QWidget::tr(lname)));
        layout->addWidget(lbl);
//        layout->addStretch();
        return lbl;
    }

    // 添加 文字+文本输入框  水平布局
    static QLineEdit* addHLabelInput(const char* lname, QHBoxLayout *layout, int w=50)
    {
        QLabel *lbl = new QLabel(QWidget::tr(lname));
        QLineEdit* edt = new QLineEdit();
        edt->setFixedWidth(w);
        layout->addWidget(lbl);
        layout->addWidget(edt);
//        layout->addStretch();
        return edt;
    }

    // 添加 文字+文本输入框  水平布局
    static QSpinBox* addHSpinInput(const char* lname, QHBoxLayout *layout, int w=60)
    {
        QLabel *lbl = new QLabel(QWidget::tr(lname));
        QSpinBox* edt = new QSpinBox();
        edt->setFixedWidth(w);
//        edt->setSingleStep(10);
//        edt->setRange(0,800);
        edt->setMaximum(8888);
//        edt->setValue(150);
        layout->addWidget(lbl);
        layout->addWidget(edt);
//        layout->addStretch();
        return edt;
    }

    static QImage cvMat2QImage(const cv::Mat& mat)
    {
        // 8-bits unsigned, NO. OF CHANNELS = 1
        if(mat.type() == CV_8UC1)
        {
            QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
            // Set the color table (used to translate colour indexes to qRgb values)
            image.setColorCount(256);
            for(int i = 0; i < 256; i++)
            {
                image.setColor(i, qRgb(i, i, i));
            }
            // Copy input Mat
            uchar *pSrc = mat.data;
            for(int row = 0; row < mat.rows; row ++)
            {
                uchar *pDest = image.scanLine(row);
                memcpy(pDest, pSrc, static_cast<size_t>(mat.cols));
                pSrc += mat.step;
            }
            return image;
        }
        // 8-bits unsigned, NO. OF CHANNELS = 3
        else if(mat.type() == CV_8UC3)
        {
            // Copy input Mat
            //const uchar *pSrc = (const uchar*)mat.data;
            // Create QImage with same dimensions as input Mat
            QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_RGB888);
            return image.rgbSwapped();
        }
        else if(mat.type() == CV_8UC4)
        {
            qDebug() << "CV_8UC4";
            // Copy input Mat
            //const uchar *pSrc = (const uchar*)mat.data;
            // Create QImage with same dimensions as input Mat
            QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_ARGB32);
            return image.copy();
        }
        else
        {
            qDebug() << "ERROR: Mat could not be converted to QImage.";
            return QImage();
        }
    }

    static cv::Mat QImage2cvMat(QImage image)
    {
        cv::Mat mat;
        uchar *pBits = const_cast<uchar*>(image.constBits());
        size_t imgstep = static_cast<size_t>(image.bytesPerLine());
        qDebug() << image.format();

        switch(static_cast<int>(image.format()))
        {
        case QImage::Format_ARGB32:
        case QImage::Format_RGB32:
            mat = cv::Mat(image.height(), image.width(), CV_8UC4, reinterpret_cast<void*>(pBits), imgstep);
            cv::cvtColor(mat, mat, CV_BGRA2BGR);
            break;
        case QImage::Format_ARGB32_Premultiplied:
            mat = cv::Mat(image.height(), image.width(), CV_8UC4, reinterpret_cast<void*>(pBits), imgstep);
            break;
        case QImage::Format_RGB888:
            mat = cv::Mat(image.height(), image.width(), CV_8UC3, reinterpret_cast<void*>(pBits), imgstep);
            cv::cvtColor(mat, mat, CV_BGR2RGB);
            break;
        case QImage::Format_Indexed8:
            mat = cv::Mat(image.height(), image.width(), CV_8UC1, reinterpret_cast<void*>(pBits), imgstep);
            break;
        }
        return mat;
    }

    // 从场景中获取图片
    static QImage *getImage(QGraphicsScene *scene)
    {
        QImage *qmg = new QImage(scene->width(), scene->height() ,QImage::Format_ARGB32);
//        QPainter *painter = new QPainter(qmg);
        QPainter painter(qmg);
        painter.setRenderHint(QPainter::Antialiasing);
        scene->render(&painter);
//        delete painter;   // 不用指针,自动释放
        return qmg;
    }

};

#endif // MYQTOOL_H
