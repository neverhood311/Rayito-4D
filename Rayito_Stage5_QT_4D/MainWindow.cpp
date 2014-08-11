#include "MainWindow.h"
#include "ui_MainWindow.h"

//#include "rayito.h"
//#include "lodepng.h"

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

    //TODO: render an RSD file that's been loaded

    //get the number of frames

    //these are temporary values, baked out from Blender
    float rot1[180] = {0.000000, 0.000000, 0.056893, 0.228158, 0.514525, 0.916501, 1.434343, 2.068030, 2.817233, 3.681286, /*10*/4.659151, 5.749395, 6.950155, 8.259113, 9.673465, 11.189910, /*16*/12.804621, 14.513235, 16.310857, 18.192040, 20.150810, 22.180684, 24.274666, 26.425322, 28.624804, 30.864884, 33.137042, 35.432522, 37.742389, 40.057618, 42.369193, 44.668102, 46.945529, 49.192826, 51.401644, 53.563966, 55.672138, 57.718958, 59.697711, 61.602173, 63.426612, 65.165851, 66.815240, 68.370638, 69.828418, 71.185481, 72.439210, 73.587413, 74.628327, 75.560634, 76.383365, 77.095883, 77.697869, 78.189342, 78.570494, 78.841802, 79.003951, 79.057766, 78.996124, 78.810493, 78.500020, 78.064179, 77.502812, 76.816187, 76.005013, 75.070548, 74.014580, 72.839528, 71.548458, 70.145134, 68.634037, 67.020425, 65.310289, 63.510391, 61.628278, 59.672166, 57.650990, 55.574285, 53.452128, 51.295035, 49.113869, 46.919732, 44.723799, 42.537243, 40.371117, 38.236154, 36.142772, 34.100891, 32.119866, 30.208441, 28.374662, 26.625884, 24.968648, 23.408841, 21.951524, 20.601073, 19.361148, 18.234747, 17.224221, 16.331372, 15.557422, 14.903159, 14.368867, 13.954473, 13.659566, 13.483374, 13.424904, 13.611710, 14.174335, 15.108020, 16.394660, 18.000096, 19.872808, 21.944933, 24.136113, 26.359817, 28.530824, 30.572210, 32.420272, 34.026869, 35.359322, 36.398635, 37.136934, 37.574746, 37.718535, 37.681426, 37.569736, 37.383053, 37.121187, 36.784155, 36.372257, 35.886070, 35.326497, 34.694776, 33.992540, 33.221801, 32.385004, 31.485027, 30.525205, 29.509320, 28.441606, 27.326747, 26.169836, 24.976368, 23.752176, 22.503400, 21.236417, 19.957789, 18.674185, 17.392307, 16.118816, 14.860282, 13.623068, 12.413316, 11.236853, 10.099177, 9.005389, 7.960169, 6.967777, 6.032010, 5.156229, 4.343358, 3.595879, 2.915862, 2.304991, 1.764569, 1.295567, 0.898655, 0.574200, 0.322310, 0.142908, 0.035633, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000};
    float rot2[180] = {0.000000, 0.000000, 0.153992, 0.618006, 1.394181, 2.483253, 3.884312, 5.594568, 7.609085, 9.920549, /*10*/12.519026, 15.391770, 18.523053, 21.894093, 25.482997, 29.264874, /*16*/33.211972, 37.293991, 41.478407, 45.731047, 50.016550, 54.299031, 58.542719, 62.712598, 66.774948, 70.697922, 74.452013, 78.010384, 81.349035, 84.447045, 87.286587, 89.852778, 92.133733, 94.120253, 95.805656, 97.185560, 98.257667, 99.021432, 99.477867, 99.629285, 99.508514, 99.144369, 98.534645, 97.677996, 96.574066, 95.223642, 93.628807, 91.793084, 89.721638, 87.421340, 84.900952, 82.171219, 79.244927, 76.136911, 72.864069, 69.445265, 65.901169, 62.254087, 58.527672, 54.746637, 50.936395, 47.122684, 43.331133, 39.586960, 35.914501, 32.336977, 28.876127, 25.552038, 22.382940, 19.385108, 16.572785, 13.958216, 11.551687, 9.361580, 7.394493, 5.655369, 4.147632, 2.873392, 1.833482, 1.027711, 0.454932, 0.113217, 0.000000, 0.227114, 0.910687, 2.052753, 3.653366, 5.710323, 8.218883, 11.171468, 14.557421, 18.362738, 22.569906, 27.157730, 32.101291, 37.371967, 42.937540, 48.762442, 54.808074, 61.033273, 67.394836, 73.848128, 80.347757, 86.848239, 93.304769, 99.673757, 105.913476, 111.984653, 117.850761, 123.478441, 128.837653, 133.901884, 138.648004, 143.056427, 147.110786, 150.797927, 154.107550, 157.032101, 159.566388, 161.707543, 163.454432, 164.807793, 165.769647, 166.343493, 166.533618, 166.307634, 165.627334, 164.490435, 162.896419, 160.846911, 158.345833, 155.399754, 152.018154, 148.213712, 144.002422, 139.403915, 134.441482, 129.142212, 123.536826, 117.659694, 111.548464, 105.243817, 98.788953, 92.229055, 85.610657, 78.981002, 72.387322, 65.876075, 59.492403, 53.279417, 47.277716, 41.524893, 36.055278, 30.899680, 26.085230, 21.635376, 17.569992, 13.905418, 10.654713, 7.827820, 5.431886, 3.471348, 1.948598, 0.863746, 0.215233, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000};

    //add some extra hypercubes
    //int cubes = 30;
    #define NUMCUBES 30
    float xrand[NUMCUBES];      //x-coordinates of the random hypercubes
    float zrand[NUMCUBES];      //z-coordinates of the random hypercubes
    float wrand[NUMCUBES];      //w-coordinates of the random hypercubes
    float sideRand[NUMCUBES];   //side lengths of the random hypercubes
    float rotRand[NUMCUBES * 6];//rotations of the random hypercubes
    for(int i = 0; i < NUMCUBES; i++){
        xrand[i] = (frand() * 8.f) - 4.f;
        zrand[i] = (frand() * 5.f) - 2.5f;
        wrand[i] = (frand() * 10.f) - 5.f;
        sideRand[i] = (frand() * 1.6f) + 0.15f;
        for(int r = 0; r < 6; r++){
            rotRand[i * 6 + r] = frand() * 180.f;
        }
    }

    Rayito::DiffuseMaterial randomLambert(Rayito::Color(0.2f, 0.7f, 0.7f));
    Rayito::Tesseract randCubes[NUMCUBES];
    //generate 30 cubes using the random numbers for position, rotation, and size
    for(int i = 0; i < NUMCUBES; i++){
        randCubes[i] = Rayito::Tesseract(Rayito::Point(xrand[i], 0, zrand[i], wrand[i]),
                                         sideRand[i],
                                         &randomLambert);
        randCubes[i].m_transform.rotate(rotRand[i * 6 + 0],
                                        rotRand[i * 6 + 1],
                                        rotRand[i * 6 + 2],
                                        rotRand[i * 6 + 3],
                                        rotRand[i * 6 + 4],
                                        rotRand[i * 6 + 5]);
    }

    //int incr = 9;
    //int frames = 90 / incr;
    time_t prevTime;
    time(&prevTime);
    double seconds;

    int frames = 180;
    frames = 2;
    // Make a picture...
    for(int i = 0; i < frames; i++){
        //i = 90;
        m_log << "Frame ";
        m_log << std::to_string(i);
        m_log << " rendering... ";
        //TODO:
        //clear the masterSet
        //load the current frame into the masterSet

        // Available materials
        Rayito::DiffuseMaterial blueishLambert(Rayito::Color(0.7f, 0.7f, 0.9f));
        Rayito::DiffuseMaterial purplishLambert(Rayito::Color(0.8f, 0.3f, 0.7f));
        Rayito::DiffuseMaterial yellowishLambert(Rayito::Color(0.7f, 0.7f, 0.2f));
        Rayito::GlossyMaterial bluishGlossy(Rayito::Color(0.5f, 0.3f, 0.8f), 0.3f);
        Rayito::GlossyMaterial greenishGlossy(Rayito::Color(0.3f, 0.9f, 0.3f), 0.0001f);

        // The 'scene'
        Rayito::ShapeSet masterSet;

        // Put a ground plane in (with bullseye texture!)
        Rayito::Plane plane(Rayito::Point(0.0f, -2.0f, 0.0f, 0.0f),
                            Rayito::Vector(0.0f, 1.0f, 0.0f, 0.0f),
                            &blueishLambert,
                            true); // Last parameter is whether to do the bullseye texture or not
        masterSet.addShape(&plane);

        // Add a pile-o-spheres with a few interesting materials

        Rayito::Sphere sphere1(Rayito::Point(3.0f, -1.0f, 0.0f, 0.0f),
                               1.0f,
                               &purplishLambert);
        //masterSet.addShape(&sphere1);

        Rayito::Sphere sphere2(Rayito::Point(-3.0f, 0.0f, -2.0f, 0.0f),
                               2.0f,
                               &greenishGlossy);
        //masterSet.addShape(&sphere2);

        Rayito::Sphere sphere3(Rayito::Point(1.5f, -1.5f, 2.5f, 0.0f),
                               0.5f,
                               &bluishGlossy);
        //masterSet.addShape(&sphere3);

        Rayito::Sphere sphere4(Rayito::Point(-2.0f, -1.5f, 1.0f, 0.0f),
                               0.5f,
                               &yellowishLambert);
        //masterSet.addShape(&sphere4);

        Rayito::Tesseract hypercube2(Rayito::Point(0.0f, 2.0f, 0.0f, -0.49f),
                          1.0f,
                          &yellowishLambert);
        hypercube2.m_transform.rotate(2 * i, 0, 2*i, 0, 0, 0);
        hypercube2.m_transform.translate(0, 2, 1, 0);
        //masterSet.addShape(&hypercube2);

        //add the random hypercubes to the masterset
        for(int c = 0; c < NUMCUBES; c++){
            masterSet.addShape(&(randCubes[c]));
        }

        Rayito::Tesseract hypercube(Rayito::Point(0.0f, 0.0f, 0.0f, 0.0f),
                          2.0f,
                          &yellowishLambert);
        //hypercube.m_transform.rotate(0, -45 + (float)(i * incr), 0, 0, 0 + (float)(i * incr), 35);//xy, xz, xw, yz, yw, zw
        hypercube.m_transform.rotate(20, 45, 0, 0, 15, 35);//xy, xz, xw, yz, yw, zw
        //hypercube.m_transform.rotate(0, 0, 20, 0, i * incr, i * incr);//xy, xz, xw, yz, yw, zw
        //hypercube.m_transform.rotate(0, rot1[i], 0, 0, rot1[i], rot2[i]);//xy, xz, xw, yz, yw, zw
        //hypercube.m_transform.translate(-3 + (6.f / 180.f) * i, 0, 0, -2 + (4.f / 180.f) * i);
        masterSet.addShape(&hypercube);

        // Add an area light
        Rayito::RectangleLight areaLight(Rayito::Point(-1.5f, 4.0f, -1.5f, 0.0f),
                                         Rayito::Vector(3.0f, 0.0f, 0.0f, 0.0f),
                                         Rayito::Vector(0.0f, 0.0f, 3.0f, 0.0f),
                                         Rayito::Color(1.0f, 1.0f, 1.0f),
                                         5.0f);
        masterSet.addShape(&areaLight);

        // Add an area light based on a shape (a sphere)
        Rayito::Sphere sphereForLight(Rayito::Point(0.0f, 0.5f, 2.0f, 0.0f),
                                      0.5f,
                                      &blueishLambert);
        Rayito::ShapeLight sphereLight(&sphereForLight, Rayito::Color(1.0f, 1.0f, 0.3f), 10.0f);
        //masterSet.addShape(&sphereLight);

        // Create the camera based on user settings
        Rayito::PerspectiveCamera cam((float)ui->camFovSpinBox->value(),
                                      Rayito::Point(0.0f, 5.0f, 15.0f, -10.0f + (20.f / 180.f) * i),   //from
                                      Rayito::Point(0.0f, 0.0f, 0.0f, -10.0f + (20.f / 180.f) * i),    //to
                                      Rayito::Point(0.0f, 1.0f, 0.0f, 0.0f),    //up
                                      (float)ui->focalDistanceSpinBox->value(),
                                      (float)ui->lensRadiusSpinBox->value());
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

    //get a filepath to an RSD file
    //validate the file
    //if not valid
        //error message and return
    //load the file into a sceneBuffer

}
