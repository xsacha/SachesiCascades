#pragma once
#include <bb/cascades/Picker>
#include <bb/cascades/PickerProvider>
#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/DockLayout>

namespace bb { namespace cascades {

    class CodePickerProvider: public bb::cascades::PickerProvider {
    public:
        CodePickerProvider() {}
        virtual ~CodePickerProvider() {};
        VisualNode * createItem(Picker * pickerList, int columnIndex) {
            Container * returnItem = Container::create().layout(new DockLayout());
            Label * desc = Label::create().objectName("desc");
            desc->setHorizontalAlignment(HorizontalAlignment::Center);
            desc->setVerticalAlignment(VerticalAlignment::Center);
            desc->textStyle()->setFontSize(FontSize::XLarge);
            returnItem->add(desc);
            return returnItem;
        }
        void updateItem(Picker * pickerList, int columnIndex, int rowIndex, VisualNode * pickerItem) {
            Container * container = (Container *)pickerItem;
            Label * desc = pickerItem->findChild<Label *>("desc");
            desc->setText(QString::number(rowIndex % 10));
        }

        int columnCount() const { return 3; }
        void range(int column, int * lowerBoundary, int * upperBoundary) {
            *lowerBoundary = 0;
            *upperBoundary = 19;
        }
        QVariant value(Picker * picker, const QList<int> & indices) const {
            int ret = 0;
            for (int i = 0; i < 3; i++)
                ret = ret * 10 + indices.value(i) % 10;

            return QVariant(ret);
        }
    };

} }
