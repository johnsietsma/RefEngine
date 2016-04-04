
#include <QApplication>
#include <QTime>

class ApplicationQT
{
public:
    ApplicationQT(int& argc, char** argv);

    float getTime() const;

    void run();

private:
    QApplication m_pApplication;
    QTime m_timer;

};
