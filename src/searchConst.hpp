#pragma once

#include <QString>

namespace SearchConst {
static const QString multi_split = ";";
static const QString newline = "\n";


namespace Col {
namespace Idx {
static const int INDEX=0;
static const int TYPE=1;
static const int SIZE=2;
static const int LINE=2;
static const int PATH=3;
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
