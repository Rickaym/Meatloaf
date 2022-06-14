#pragma once

template <class T>
struct Result
{
    bool failed;
    T value;
    std::unique_ptr<BaseException> error;

    static Result success(T& val)
    {
        return Result<T>(val);
    }

    static Result success(T&& val)
    {
        return Result<T>(val);
    }

    static Result failure(std::unique_ptr<BaseException>& fault)
    {
        return Result<T>(fault);
    }

    static Result failure(std::unique_ptr<BaseException>&& fault)
    {
        return Result<T>(fault);
    }
private:
    Result(T& val)
        : value(std::move(val)), failed(false) {};

    Result(std::unique_ptr<BaseException>& fault)
        : error(std::move(fault)), failed(true) {};
};
