#pragma once

namespace muduo
{
    //@ tag class,标记类型可拷贝
    //@ EBO (空基类优化)
    //@ 以此为基类的子类必须是值类型的
    class Copyable
    {
    protected:
        Copyable() = default;
        ~Copyable() = default;
    };
}