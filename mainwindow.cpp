#include "mainwindow.h"

const QString MainWindow::foundResultText = "Result was found for ";
const QString MainWindow::notFoundResultText = "Result was not found for ";
const QString MainWindow::notObjectFoundText = "Choose the object's image!";

void MainWindow::hough(const QImage object, ImageAndResult& sceneContainer) {
    sceneContainer.tryToFind = true;
    const MyImage objectImage = MyImage::createMyImageFromQImage(object);
    const auto firstVector = Descriptor::buildDescriptors(objectImage);

    const MyImage sceneImage = MyImage::createMyImageFromQImage(sceneContainer.original);
    auto secondVector = Descriptor::buildDescriptors(sceneImage);

    auto matches = DescriptorMatcher::findMatchersBetweenDescriptors(secondVector, firstVector);
    for (PointMatch& match : matches) {
        match.swapPoints();
    }
    if (matches.size() >= BORDER_TO_FIND_HOUGH_OBJECT) {
        const int widthFirst = object.width(), heightFirst = object.height();
        HoughTransforamtion transformation = HoughAlgorithm::getObjectsParameters(
                    matches,
                    heightFirst, widthFirst,
                    sceneContainer.original.height(), sceneContainer.original.width()
                    );
        if (transformation.found) {
            sceneContainer.foundResult = true;
            const int centerInSecondX = transformation.x,
                      centerInSecondY = transformation.y;
            const int sizeInSecondX = widthFirst * transformation.scale,
                      sizeInSecondY = heightFirst * transformation.scale;
            QImage result = QImage(sceneContainer.original);
            QPainter painter(&result);
            painter.drawEllipse(QPointF(centerInSecondX, centerInSecondY), 10, 10);
            QRect rect = QRect(centerInSecondX - sizeInSecondX / 2, centerInSecondY - sizeInSecondY / 2,
                               sizeInSecondX, sizeInSecondY);
            painter.drawRect(rect);
            sceneContainer.result = result;
        }
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->findFileButton, SIGNAL(clicked(bool)), this, SLOT(onFindFileButtonClicked()));
    connect(ui->fildFolderButton, SIGNAL(clicked(bool)), this, SLOT(onFindFolderButtonClicked()));
    connect(ui->sceneFilesList, SIGNAL(clicked(QModelIndex)), this, SLOT(clickOnItem(QModelIndex)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onFindFileButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Find file with object"), "..", tr("Jpg files (*.jpg)"));
    const QPixmap objectPixmap = QPixmap(fileName);
    ui->objectLabel->setPixmap(objectPixmap);
    object = objectPixmap.toImage();
    for (ImageAndResult container : images) {
        container.tryToFind = false;
        container.foundResult = false;
    }
}

void MainWindow::onFindFolderButtonClicked() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory("..");
    int result = dialog.exec();
    if (result) {
        ui->sceneFilesList->clear();
        images.clear();
        QString directoryPath = dialog.selectedFiles()[0];
        QDir directory = QDir(directoryPath);
        foreach (const QString& filename, directory.entryList(QStringList("*.jpg"), QDir::Files)) {
            ui->sceneFilesList->addItem(filename);
            QString absolutePath = directory.absoluteFilePath(filename);
            qDebug() << absolutePath;
            images.emplace_back(filename, QPixmap(absolutePath).toImage());
        }
    }
}

void MainWindow::clickOnItem(const QModelIndex &index) {
    if (object.isNull()) {
        QMessageBox box;
        box.setText(notObjectFoundText);
        box.exec();
        return;
    }
    const int indexOfObject = index.row();
    ImageAndResult& container = images.at(indexOfObject);
    if (!container.tryToFind) {
        hough(object, container);
    }
    QGraphicsScene* scene = new QGraphicsScene(this);
    if (container.foundResult) {
        ui->showResultLabel->setText(foundResultText + container.filename);
        scene->addPixmap(QPixmap::fromImage(container.result));
    } else {
        ui->showResultLabel->setText(notFoundResultText + container.filename);
        scene->addPixmap(QPixmap::fromImage(container.original));
    }
    ui->resultView->setScene(scene);
    ui->resultView->show();
}
