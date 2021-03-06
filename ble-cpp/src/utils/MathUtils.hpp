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

#ifndef MathUtils_hpp
#define MathUtils_hpp

#include <cmath>
#include <boost/math/distributions/students_t.hpp>

class DirectionalStatistics{
    double mCircularMean;
    double mCircularVariance;
    
public:
    DirectionalStatistics(double theta, double v){
        this->mCircularMean = theta;
        this->mCircularVariance = v;
    }
    double circularMean() const{
        return mCircularMean;
    }
    double circularVariance() const{
        return mCircularVariance;
    }
};

class NormalParameter{
protected:
    double mMean;
    double mStdev;
public:
    NormalParameter() = default;
    NormalParameter(double mean, double stdev){
        this->mMean = mean;
        this->mStdev = stdev;
    }
    ~NormalParameter() = default;
    double mean() const{return mMean;}
    double stdev() const{return mStdev;}
};

using WrappedNormalParameter = NormalParameter;

class MathUtils{
    
public:
    static double probaNormal(double x, double mu, double sigma){
        return 1.0/(std::sqrt(2.0*M_PI)*sigma)*std::exp(-std::pow(x-mu, 2)/(2*sigma*sigma));
    }
    
    static double logProbaNormal(double x, double mu, double sigma){
        return -1.0/2.0*std::log(2*M_PI) -1.0/2.0*std::log(sigma*sigma)
        -std::pow(x-mu, 2)/(2.0*sigma*sigma);
    }
    
    static std::function<double(double,double,double)> logProbatDistFunc(double nu) {
        boost::math::students_t dist(nu);
        return [=] (double x, double mu, double sigma){
            double z = (x - mu) / sigma;
            return std::log(boost::math::pdf(dist, z));
        };
    }
    
    static double mahalanobisDistance(double x, double mu, double sigma){
        return std::pow(x-mu, 2)/(sigma*sigma);
    }
    
    static double quantileChiSquaredDistribution(int degreeOfFreedom, double cumulativeDensity);
    
    static double normalizeOrientaion(double orientation){
        double x = std::cos(orientation);
        double y = std::sin(orientation);
        return std::atan2(y,x);
    }
    
    static DirectionalStatistics computeDirectionalStatistics(std::vector<double> orientations);
    static WrappedNormalParameter computeWrappedNormalParameters(const std::vector<double>& orientations);
};
#endif /* MathUtils_hpp */
