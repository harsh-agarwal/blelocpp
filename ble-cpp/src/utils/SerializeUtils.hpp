/*******************************************************************************
 * Copyright (c) 2014, 2015  IBM Corporation and others
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/

#ifndef EigenSerializeUtils_h
#define EigenSerializeUtils_h

#ifdef ANDROID_STL_EXT
#include "string_ext.hpp"
#endif /* ANDROID_STL_EXT */
#include <Eigen/Core>
#include <Eigen/SparseCore>

#include <cereal/archives/xml.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>

namespace cereal{
    /**
     Funtions to save/load Eigen::Matrix
     **/
    template<class Archive, class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
    void save(Archive& archive, Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> const & X){
        int rows = static_cast<int>(X.rows());
        int cols = static_cast<int>(X.cols());
        archive(CEREAL_NVP(rows));
        archive(CEREAL_NVP(cols));
        std::vector<double> elements(rows*cols);
        Eigen::Map<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>>(&elements[0], rows, cols ) = X;
        archive( CEREAL_NVP(elements));
    }
    
    template<class Archive, class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
    void load(Archive & archive, Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> & X)
    {
        int rows;
        int cols;
        archive(CEREAL_NVP(rows));
        archive(CEREAL_NVP(cols));
        std::vector<double> elements(rows*cols);
        archive(CEREAL_NVP(elements));
        X = Eigen::Map<Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>>(&elements[0], rows, cols);
    }
    
    /**
     Funtions to save/load Eigen::SparseMatrix
     **/
    template<class Archive, class _Scalar, int _Options, class _StorageIndex>
    void save(Archive& ar, Eigen::SparseMatrix<_Scalar, _Options, _StorageIndex> const & X, const std::uint32_t version){
        // column-major
        typename Eigen::SparseMatrix<_Scalar, _Options, _StorageIndex>::Index rows = X.rows(), cols = X.cols(), nonZeros = X.nonZeros();
        ar(CEREAL_NVP(rows));
        ar(CEREAL_NVP(cols));
        ar(CEREAL_NVP(nonZeros));

        std::vector<int> rowIndices(nonZeros);
        std::vector<int> colIndices(nonZeros);
        std::vector<_Scalar> values(nonZeros);

        int index = 0;
        for (int k=0; k<X.outerSize(); ++k){
            for (typename Eigen::SparseMatrix<_Scalar, _Options, _StorageIndex>::InnerIterator it(X,k); it; ++it){
                rowIndices[index] = it.row();
                colIndices[index] = it.col();
                values[index] = it.value();
                index++;
            }
        }

        ar(CEREAL_NVP(rowIndices));
        ar(CEREAL_NVP(colIndices));
        ar(CEREAL_NVP(values));
    }

    template<class Archive, class _Scalar, int _Options, class _StorageIndex>
    void load(Archive& ar, Eigen::SparseMatrix<_Scalar, _Options, _StorageIndex> & X, const std::uint32_t version){
        // column-major
        typename Eigen::SparseMatrix<_Scalar, _Options, _StorageIndex>::Index rows = X.rows(), cols = X.cols(), nonZeros = X.nonZeros();
        ar(CEREAL_NVP(rows));
        ar(CEREAL_NVP(cols));
        ar(CEREAL_NVP(nonZeros));

        std::vector<int> rowIndices(nonZeros);
        std::vector<int> colIndices(nonZeros);
        std::vector<_Scalar> values(nonZeros);

        ar(CEREAL_NVP(rowIndices));
        ar(CEREAL_NVP(colIndices));
        ar(CEREAL_NVP(values));

        X.resize(rows, cols);
        using Triplet = typename Eigen::Triplet<_Scalar>;
        std::vector<Triplet> tripletList;

        for(int i=0; i<nonZeros; i++){
            tripletList.push_back(Triplet(rowIndices.at(i), colIndices.at(i), values.at(i)));
        }
        X.setFromTriplets(tripletList.begin(), tripletList.end());
        X.makeCompressed();
        X.finalize();
    }


    template <class Archive, class T>
    void make_optional_nvp(Archive &ar, const std::string &name, T &value) {
        try {
            ar(cereal::make_nvp(name, value));
        } catch (cereal::Exception& e) {
            std::cerr << "NOTICE: provided NVP not found (name=" << name << ")" << std::endl;
        }
    }
    #define     OPTIONAL_NVP(A, T)   make_optional_nvp(A, #T, T)
}

CEREAL_CLASS_VERSION(Eigen::SparseMatrix<uint8_t>, 0);

#endif /* EigenSerializeUtils_h */
