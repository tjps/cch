#pragma once


class interface {
public:

    virtual void doSomething() = 0;
    virtual int calcSomething(int x, int y) = 0;
};

class impl : public interface {
public:

    virtual void doSomething();
    virtual int calcSomething(int x, int y) override;
};
