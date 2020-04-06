#pragma once

#include <boost/yap/expression.hpp>
#include <boost/hana.hpp>
#include <boost/yap/print.hpp>
#include <iostream>
#include <cstdio>
#include "xforms/Placeholder.hpp"
#include "xforms/PrintIR.hpp"
#include "Tensor.hpp"
#include "TensorOps.hpp"
#include "XformUtils.hpp"

struct CodeGenXform {
    template <yap::expr_kind BinaryOP, typename Dest, typename Src1, typename Src2>
    auto operator()(yap::expr_tag<yap::expr_kind::assign>,
            Dest const &dest_expr,
            yap::expression<BinaryOP, hana::tuple<Src1, Src2>> const &bin_expr) {
        auto dest = yap::value(dest_expr);
        auto src1 = yap::value(yap::left(bin_expr));
        auto src2 = yap::value(yap::right(bin_expr));

        static_assert(is_tensor<std::remove_reference_t<decltype(dest)> >);
        static_assert(is_tensor<std::remove_reference_t<decltype(src1)> >);
        static_assert(is_tensor<std::remove_reference_t<decltype(src2)> >);

//        printf("CodeGenXform: tensor = expr1 op expr2 matched\n");

        if constexpr (BinaryOP == yap::expr_kind::plus) {

            auto tadd = TAdd(dest, src1, src2);
            auto code = tadd.gen_code();
            return code;
            //auto code = TAdd(dest, src1, src2).gen_code();
            //return code;
//            std::cout << "Executing TensorAdd`" << std::endl;
        }
//        else if constexpr (BinaryOP == yap::expr_kind::multiplies) {
//            std::cout << "Executing TensorMul" << std::endl;
//        }
        else {
            return []() {
                printf("Not implemented\n");
            };
        }
    }
};

struct CodeGen : StaticTransform {
    using tag = xform_pass_tag;

    // Given an IRList, returns the codes generated
    template <typename IRList>
    constexpr auto transform(IRList &&irlist) const {
        auto codes = hana::transform(irlist, [](auto &&ir) {
            return yap::transform(ir, CodeGenXform());
        });
        return codes;
    }
};