#pragma once

#include <QString>

namespace RapidNotes {

class WordBucket {
public:
    static int calculate(const QString& content, const QString& itemType);

private:
    WordBucket() = default;
};

} // namespace RapidNotes
