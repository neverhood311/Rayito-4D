#include "MainWindow.h"
#include "ui_MainWindow.h"

//#include "rayito.h"
//#include "lodepng.h"

#include "SceneLoader.h"
#include <QGraphicsScene>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *pParent)
    : QMainWindow(pParent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // We have to allocate this, because Qt doesn't do it for us by default.
    // Oftentimes you would create your own QGraphicsScene subclass, but we don't.
    QGraphicsScene *pScene = new QGraphicsScene(this);
    ui->renderGraphicsView->setScene(pScene);

    //set pImage to NULL
    pImage = NULL;
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionQuit_triggered()
{
    qApp->quit();
}

float frand(){
    return float(rand()) / float(RAND_MAX);
}

void MainWindow::on_renderButton_clicked()
{
    //open a log file
    std::ofstream m_log;
    m_log.open("log.txt", std::ios::app);
    if(m_log.is_open()){
        time_t rawtime;
        time(&rawtime);
        m_log << ctime(&rawtime);
        m_log << "Render started \n";
        //log a few things about the render
        m_log << ui->widthSpinBox->value() << "x" << ui->heightSpinBox->value() << "\n";
        m_log << ui->pixelSamplesSpinBox->value() << " pixel samples\n";
        m_log << ui->lightSamplesSpinBox->value() << " light samples\n";
        m_log << ui->rayDepthSpinBox->value() << " bounces max\n";
    }




    time_t prevTime;
    time(&prevTime);
    double seconds;


    //render an RSD file that's been loaded
    //clear the master set
    //get the next frame and load it into the master set

    //get the number of frames
    int startFrame = buf->m_renderSettings.startFrame;
    int endFrame = buf->m_renderSettings.endFrame;
    int frames = endFrame - startFrame;
    frames = 2;
    // Make a picture...
    for(int i = 0; i < frames; i++){
        //i = 90;
        m_log << "Frame ";
        m_log << std::to_string(i);
        m_log << " rendering... ";
        //clear the masterSet
        //load the current frame into the masterSet
        Rayito::ShapeSet masterSet;
        std::map<std::string, Rayito::Shape*>::iterator iter = buf->m_shapes.begin();
        for(; iter != buf->m_shapes.end(); ++iter){
            //get the shape
            //apply the frame's transforms
            Rayito::Shape* tempShape = iter->second;
            masterSet.addShape(iter->second);
        }

        //clear out pImage if there's one there already
        if(pImage != NULL){
            delete pImage;  //otherwise, you have a memory leak
        }

        // Ray trace!
        pImage = raytrace(masterSet,
                                         cam,
                                         ui->widthSpinBox->value(),
                                         ui->heightSpinBox->value(),
                                         ui->pixelSamplesSpinBox->value(),
                                         ui->lightSamplesSpinBox->value(),
                                         ui->rayDepthSpinBox->value(),
                                         i);

        // Convert from floating-point RGB to 32-bit ARGB format,
        // applying exposure and gamma along the way

        // A gamma'd value is value^(1/gamma)
        float gammaExponent = 1.0f / (float)ui->gammaSpinBox->value();
        // An exposure'd value is value*2^exposure (applied before gamma)
        float exposure = std::pow(2.0f, (float)ui->exposureSpinBox->value());
        uchar *argbPixels = new uchar[pImage->width() * pImage->height() * 4];

        std::vector<unsigned char> pngImage;
        pngImage.resize(pImage->width() * pImage->height() * 4);

        for (size_t y = 0; y < pImage->height(); ++y)
        {
            for (size_t x = 0; x < pImage->width(); ++x)
            {
                size_t pixelOffset = (y * pImage->width() + x) * 4;
                Rayito::Color color = pImage->pixel(x, y);
                // Check for negative values (we don't like those).  Make them green.
                if (color.m_r < 0.0f || color.m_g < 0.0f || color.m_b < 0.0f)
                {
                    color = Rayito::Color(0.0f, 1.0f, 0.0f);
                }
                else
                {
                    // Combined gamma+exposure: result = (value*(2^exposure))^(1/gamma)
                    color.m_r = std::pow(color.m_r * exposure, gammaExponent);
                    color.m_g = std::pow(color.m_g * exposure, gammaExponent);
                    color.m_b = std::pow(color.m_b * exposure, gammaExponent);
                    // Check for NaNs (not-a-number), we HATE those.  Make them blue.
                    if (color.m_r != color.m_r || color.m_g != color.m_g || color.m_b != color.m_b)
                    {
                        color = Rayito::Color(0.0f, 0.0f, 1.0f);
                    }
                }
                // We're displaying in LDR in the end (you've got an exposure control, after all)
                color.clamp();
                // Stuff actual 32-bit ARGB pixel data in
                argbPixels[pixelOffset + 3] = 0xFF;
                argbPixels[pixelOffset + 2] = static_cast<uchar>(color.m_r * 255.0f);
                argbPixels[pixelOffset + 1] = static_cast<uchar>(color.m_g * 255.0f);
                argbPixels[pixelOffset + 0] = static_cast<uchar>(color.m_b * 255.0f);

                //this one is for saving out to .png
                pngImage[pixelOffset + 3] = 255;
                pngImage[pixelOffset + 2] = static_cast<uchar>(color.m_b * 255.0f);
                pngImage[pixelOffset + 1] = static_cast<uchar>(color.m_g * 255.0f);
                pngImage[pixelOffset + 0] = static_cast<uchar>(color.m_r * 255.0f);
            }
        }

        // Make an image, then make a pixmap for the graphics scene
        QImage image(argbPixels,
                     static_cast<int>(pImage->width()),
                     static_cast<int>(pImage->height()),
                     QImage::Format_ARGB32_Premultiplied);

        ui->renderGraphicsView->scene()->clear();
        ui->renderGraphicsView->scene()->addPixmap(QPixmap::fromImage(image));

        // Clean up the image and pixel conversion buffers
        delete[] argbPixels;
        //delete pImage;

        masterSet.clearShapes();

        //save the image
        //std::string _path = "C:/Users/Burton/Documents/GitHub/Rayito/frame" + std::to_string(i) + ".png";
        std::string _path = "frame" + std::to_string(i) + ".png";
        //printf(_path.c_str());
        lodepng::encode(_path, pngImage, pImage->width(), pImage->height());

        //record the time
        time_t curTime;
        time(&curTime);
        seconds = difftime(curTime, prevTime);
        m_log << seconds;
        m_log << " seconds\n";
        prevTime = curTime;
    }
    m_log << "File saved successfully\n===================================\n";
    m_log.close();
}

void MainWindow::on_actionRender_Scene_triggered()
{
   
}

void MainWindow::on_actionSave_as_triggered(){
    //QString filename = QFileDialog::getOpenFileName(this, tr("Save Image"), "", "All files(*.*);;XML Files (*.xml)");
    if(pImage == NULL){
        QMessageBox::warning(this, tr("No render"), "There's no render to save");
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), "untitled.png", "PNG Image (*.png)");

    if(filename == ""){
        QMessageBox::warning(this, tr("Save failed"), "No file selected");
        return;
    }

    //try to save the image
    // A gamma'd value is value^(1/gamma)
    float gammaExponent = 1.0f / (float)ui->gammaSpinBox->value();
    // An exposure'd value is value*2^exposure (applied before gamma)
    float exposure = std::pow(2.0f, (float)ui->exposureSpinBox->value());
    //uchar *argbPixels = new uchar[pImage->width() * pImage->height() * 4];
    std::vector<unsigned char> pngImage;
    pngImage.resize(pImage->width() * pImage->height() * 4);
    for (size_t y = 0; y < pImage->height(); ++y)
    {
        for (size_t x = 0; x < pImage->width(); ++x)
        {
            size_t pixelOffset = (y * pImage->width() + x) * 4;
            Rayito::Color color = pImage->pixel(x, y);
            // Check for negative values (we don't like those).  Make them green.
            if (color.m_r < 0.0f || color.m_g < 0.0f || color.m_b < 0.0f)
            {
                color = Rayito::Color(0.0f, 1.0f, 0.0f);
            }
            else
            {
                // Combined gamma+exposure: result = (value*(2^exposure))^(1/gamma)
                color.m_r = std::pow(color.m_r * exposure, gammaExponent);
                color.m_g = std::pow(color.m_g * exposure, gammaExponent);
                color.m_b = std::pow(color.m_b * exposure, gammaExponent);
                // Check for NaNs (not-a-number), we HATE those.  Make them blue.
                if (color.m_r != color.m_r || color.m_g != color.m_g || color.m_b != color.m_b)
                {
                    color = Rayito::Color(0.0f, 0.0f, 1.0f);
                }
            }
            // We're displaying in LDR in the end (you've got an exposure control, after all)
            color.clamp();
            // Stuff actual 32-bit ARGB pixel data in
            pngImage[pixelOffset + 3] = 255;
            pngImage[pixelOffset + 2] = static_cast<uchar>(color.m_b * 255.0f);
            pngImage[pixelOffset + 1] = static_cast<uchar>(color.m_g * 255.0f);
            pngImage[pixelOffset + 0] = static_cast<uchar>(color.m_r * 255.0f);
        }
    }
    std::string _path = filename.toLocal8Bit().constData();
    lodepng::encode(_path, pngImage, pImage->width(), pImage->height());

    //alert status of saved image
    QMessageBox::information(this, tr("File Name"), filename);

    //save the image to this filepath
}

void MainWindow::on_actionLoad_triggered(){
    printf("loaded something\n");

    QString filename = QFileDialog::getOpenFileName(this, tr("Load Scene"), "", "RSD files(*.rsd)");

    if(filename == "")return;
    //get a filepath to an RSD file
    //validate the file
    buf = new SceneBuffer();
    std::string theFilename = "filename.rsd";
    RSDLoader* loader = new RSDLoader(buf, filename.toLocal8Bit().constData());
    //if not valid
    if(!loader->noErrors){
        //error message and return
        QMessageBox::warning(this, tr("Load failed"), "The Rsd file has an error!");
        return;
    }

    QMessageBox::warning(this, tr("Load succeeded"), "The Rsd file has been loaded!");
    //load the file into a sceneBuffer
    loader->Load();
}
