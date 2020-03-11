#pragma once

#include <QString>

namespace SearchConst {
static const QString multi_split = ";";
static const QString newline = "\n";


namespace Col {
namespace Idx {
static const int INDEX=0;
static const int TYPE=1;
static const int SIZE_byte=2;
static const int SIZE_human=3;
static const int LINE=4;
static const int PATH=5;
}
namespace Head {
static const QString INDEX="序号";
static const QString PATH="路径";
static const QString TYPE="类型";
static const QString SIZE="大小";
static const QString LINE="所在行";
}
}
}
