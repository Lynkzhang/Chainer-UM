#include "chainerx/native/native_device.h"

#include <cstdint>

#include "chainerx/arithmetic_ops.h"
#include "chainerx/array.h"
#include "chainerx/device.h"
#include "chainerx/dtype.h"
#include "chainerx/float16.h"
#include "chainerx/native/elementwise.h"
#include "chainerx/native/op_regist.h"
#include "chainerx/routines/math.h"
#include "chainerx/scalar.h"

namespace chainerx {
namespace native {
namespace {

class NativeAddOp : public AddOp {
public:
    void Call(const Array& x1, const Array& x2, const Array& out) override {
        Device& device = x1.device();
        device.CheckDevicesCompatible(x1, x2, out);
        const Array& x1_cast = x1.dtype() == out.dtype() ? x1 : x1.AsType(out.dtype());
        const Array& x2_cast = x2.dtype() == out.dtype() ? x2 : x2.AsType(out.dtype());
        VisitDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x1, T x2, T& out) { out = ArithmeticOps<T>::Add(x1, x2); }
            };
            Elementwise<const T, const T, T>(Impl{}, x1_cast, x2_cast, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_OP(AddOp, NativeAddOp);

class NativeAddASOp : public AddASOp {
public:
    void Call(const Array& x1, Scalar x2, const Array& out) override {
        Device& device = x1.device();
        device.CheckDevicesCompatible(x1, out);
        const Array& x1_cast = x1.dtype() == out.dtype() ? x1 : x1.AsType(out.dtype());
        VisitDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x1, T& out) { out = ArithmeticOps<T>::Add(x1, x2); }
                T x2;
            };
            Elementwise<const T, T>(Impl{static_cast<T>(x2)}, x1_cast, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_OP(AddASOp, NativeAddASOp);

class NativeSubtractOp : public SubtractOp {
public:
    void Call(const Array& x1, const Array& x2, const Array& out) override {
        Device& device = x1.device();
        device.CheckDevicesCompatible(x1, x2, out);
        const Array& x1_cast = x1.dtype() == out.dtype() ? x1 : x1.AsType(out.dtype());
        const Array& x2_cast = x2.dtype() == out.dtype() ? x2 : x2.AsType(out.dtype());
        VisitNumericDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x1, T x2, T& out) { out = ArithmeticOps<T>::Subtract(x1, x2); }
            };
            Elementwise<const T, const T, T>(Impl{}, x1_cast, x2_cast, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_OP(SubtractOp, NativeSubtractOp);

class NativeSubtractASOp : public SubtractASOp {
public:
    void Call(const Array& x1, Scalar x2, const Array& out) override {
        Device& device = x1.device();
        device.CheckDevicesCompatible(x1, out);
        const Array& x1_cast = x1.dtype() == out.dtype() ? x1 : x1.AsType(out.dtype());
        VisitNumericDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x1, T& out) { out = ArithmeticOps<T>::Subtract(x1, x2); }
                T x2;
            };
            Elementwise<const T, T>(Impl{static_cast<T>(x2)}, x1_cast, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_OP(SubtractASOp, NativeSubtractASOp);

class NativeMultiplyOp : public MultiplyOp {
public:
    void Call(const Array& x1, const Array& x2, const Array& out) override {
        Device& device = x1.device();
        device.CheckDevicesCompatible(x1, x2, out);
        const Array& x1_cast = x1.dtype() == out.dtype() ? x1 : x1.AsType(out.dtype());
        const Array& x2_cast = x2.dtype() == out.dtype() ? x2 : x2.AsType(out.dtype());
        VisitDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x1, T x2, T& out) { out = ArithmeticOps<T>::Multiply(x1, x2); }
            };
            Elementwise<const T, const T, T>(Impl{}, x1_cast, x2_cast, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_OP(MultiplyOp, NativeMultiplyOp);

class NativeMultiplyASOp : public MultiplyASOp {
public:
    void Call(const Array& x1, Scalar x2, const Array& out) override {
        Device& device = x1.device();
        device.CheckDevicesCompatible(x1, out);
        const Array& x1_cast = x1.dtype() == out.dtype() ? x1 : x1.AsType(out.dtype());
        VisitDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x1, T& out) { out = ArithmeticOps<T>::Multiply(x1, x2); }
                T x2;
            };
            Elementwise<const T, T>(Impl{static_cast<T>(x2)}, x1_cast, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_OP(MultiplyASOp, NativeMultiplyASOp);

int32_t FloorDivide(int32_t x, int32_t y) {
    auto div = std::div(x, y);
    return div.quot - ((y >= 0 ? div.rem : -div.rem) < 0 ? 1 : 0);
}
int64_t FloorDivide(int64_t x, int64_t y) {
    auto div = std::div(x, y);
    return div.quot - ((y >= 0 ? div.rem : -div.rem) < 0 ? 1 : 0);
}
int8_t FloorDivide(int8_t x, int8_t y) { return static_cast<int8_t>(FloorDivide(static_cast<int32_t>(x), static_cast<int32_t>(y))); }
int16_t FloorDivide(int16_t x, int16_t y) { return static_cast<int16_t>(FloorDivide(static_cast<int32_t>(x), static_cast<int32_t>(y))); }
uint8_t FloorDivide(uint8_t x, uint8_t y) { return x / y; }
float FloorDivide(float x, float y) {
    float rem = std::fmod(x, y);
    return (x - rem) / y - ((rem < 0 && y > 0) || (rem > 0 && y < 0) ? 1 : 0);
}
double FloorDivide(double x, double y) {
    double rem = std::fmod(x, y);
    return (x - rem) / y - ((rem < 0 && y > 0) || (rem > 0 && y < 0) ? 1 : 0);
}
chainerx::Float16 FloorDivide(chainerx::Float16 x, chainerx::Float16 y) {
    return chainerx::Float16{FloorDivide(static_cast<float>(x), static_cast<float>(y))};
}

class NativeFloorDivideOp : public FloorDivideOp {
public:
    void Call(const Array& x1, const Array& x2, const Array& out) override {
        Device& device = x1.device();
        device.CheckDevicesCompatible(x1, x2, out);
        const Array& x1_cast = x1.dtype() == out.dtype() ? x1 : x1.AsType(out.dtype());
        const Array& x2_cast = x2.dtype() == out.dtype() ? x2 : x2.AsType(out.dtype());
        VisitNumericDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x1, T x2, T& out) { out = native::FloorDivide(x1, x2); }
            };
            Elementwise<const T, const T, T>(Impl{}, x1_cast, x2_cast, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_OP(FloorDivideOp, NativeFloorDivideOp);

class NativeFloorDivideASOp : public FloorDivideASOp {
public:
    void Call(const Array& x1, Scalar x2, const Array& out) override {
        Device& device = x1.device();
        device.CheckDevicesCompatible(x1, out);
        const Array& x1_cast = x1.dtype() == out.dtype() ? x1 : x1.AsType(out.dtype());
        VisitNumericDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x1, T& out) { out = native::FloorDivide(x1, x2); }
                T x2;
            };
            Elementwise<const T, T>(Impl{static_cast<T>(x2)}, x1_cast, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_OP(FloorDivideASOp, NativeFloorDivideASOp);

class NativeDivideOp : public DivideOp {
public:
    void Call(const Array& x1, const Array& x2, const Array& out) override {
        Device& device = x1.device();
        device.CheckDevicesCompatible(x1, x2, out);
        const Array& x1_cast = x1.dtype() == out.dtype() ? x1 : x1.AsType(out.dtype());
        const Array& x2_cast = x2.dtype() == out.dtype() ? x2 : x2.AsType(out.dtype());
        VisitDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x1, T x2, T& out) { out = ArithmeticOps<T>::Divide(x1, x2); }
            };
            Elementwise<const T, const T, T>(Impl{}, x1_cast, x2_cast, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_OP(DivideOp, NativeDivideOp);

class NativeDivideASOp : public DivideASOp {
public:
    void Call(const Array& x1, Scalar x2, const Array& out) override {
        Device& device = x1.device();
        device.CheckDevicesCompatible(x1, out);
        const Array& x1_cast = x1.dtype() == out.dtype() ? x1 : x1.AsType(out.dtype());
        VisitDtype(out.dtype(), [&](auto pt) {
            using T = typename decltype(pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, T x1, T& out) { out = ArithmeticOps<T>::Divide(x1, x2); }
                T x2;
            };
            Elementwise<const T, T>(Impl{static_cast<T>(x2)}, x1_cast, out);
        });
    }
};

CHAINERX_NATIVE_REGISTER_OP(DivideASOp, NativeDivideASOp);

}  // namespace
}  // namespace native
}  // namespace chainerx
