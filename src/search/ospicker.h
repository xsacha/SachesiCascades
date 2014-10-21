#pragma once
#include <bb/cascades/Picker>
#include <bb/cascades/PickerProvider>
#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/DockLayout>

namespace bb { namespace cascades {

    class OSPickerProvider: public bb::cascades::PickerProvider {
    public:
        OSPickerProvider() {}
        virtual ~OSPickerProvider() {};
        VisualNode * createItem(Picker * pickerList, int columnIndex) {
            Q_UNUSED(pickerList);
            Container * returnItem = Container::create().layout(new DockLayout());
            Label * desc = Label::create().objectName("desc");
            desc->setHorizontalAlignment(HorizontalAlignment::Center);
            desc->setVerticalAlignment(VerticalAlignment::Center);
            if (columnIndex == 0)
                desc->textStyle()->setFontSize(FontSize::Medium);
            else if (columnIndex == 1)
                desc->textStyle()->setFontSize(FontSize::Medium);
            else
                desc->textStyle()->setFontSize(FontSize::XLarge);
            returnItem->add(desc);
            return returnItem;
        }
        void updateItem(Picker * pickerList, int columnIndex, int rowIndex, VisualNode * pickerItem) {
            Q_UNUSED(pickerList);
            //Container * container = (Container *)pickerItem;
            Label * desc = pickerItem->findChild<Label *>("desc");
            if (columnIndex == 0)
                desc->setText(QString("10.%1.").arg(rowIndex));
            else if (columnIndex == 1)
                desc->setText(QString("%1.").arg(rowIndex % 5));
            else if (columnIndex == 2)
                desc->setText(QString("%1").arg(rowIndex));
            else
                desc->setText(QString::number(rowIndex % 10));
        }

        int columnCount() const { return 5; }
        void range(int column, int * lowerBoundary, int * upperBoundary) {
            if (column == 0) {
                *lowerBoundary = 0;
                *upperBoundary = 6;
            } else if (column == 1) {
                *lowerBoundary = 0;
                *upperBoundary = 4 * 2 + 1;
            } else if (column == 2) {
                *lowerBoundary = 0;
                *upperBoundary = 50;
            } else {
                *lowerBoundary = 0;
                *upperBoundary = 9 * 2 + 1;
            }
        }
        QVariant value(Picker * picker, const QList<int> & indices) const {
            Q_UNUSED(picker);
            QString val = QString("10.%1.%2.%3%4%5").arg(indices.value(0)).arg(indices.value(1) % 5).arg(indices.value(2)).arg(indices.value(3) % 10).arg(indices.value(4) % 10);

            return QVariant(val);
        }
    };

} }
