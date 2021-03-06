/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2016 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    geometryWENO-Test

Description
    Test geometryWENO class with Catch2
    
Author
    Jan Wilhelm Gärtner <jan.gaertner@outlook.de>

\*---------------------------------------------------------------------------*/

#include "catch.hpp"

#include "fvCFD.H"
#include "geometryWENO.H"
#include <cmath>


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

TEST_CASE("geometryWENO: Jacobi Matrix","[3D]")
{
    using scalarSquareMatrix = SquareMatrix<scalar>;
    
    pointField pts(4,vector(0,0,0));
    
    // Populate points
    pts[0] =vector(0,0,0);
    pts[1] =vector(1,0,0);
    pts[2] =vector(0,2,0);
    pts[3] =vector(0,0,3);
    
    labelList referenceFrame(4);
    std::iota(referenceFrame.begin(), referenceFrame.end(),0);
    
    SECTION("Jacobi with reference frame")
    {
        scalarSquareMatrix J = geometryWENO::jacobi(pts,referenceFrame);
        
        for (int i = 0; i<J.n();i++)
        {
            for (int j = 0; j < J.n(); j++)
            {
                if (i==j)
                    REQUIRE(Approx(J(i,j)) == i+1);
                else
                    REQUIRE(Approx(J(i,j)) == 0);
            }
        }
    }
    
    SECTION("Jacobi with given points")
    {
        // Create Jacobi from components
        scalarSquareMatrix J = geometryWENO::jacobi
        (
            pts[referenceFrame[0]][0], pts[referenceFrame[0]][1],
            pts[referenceFrame[0]][2], pts[referenceFrame[1]][0],
            pts[referenceFrame[1]][1], pts[referenceFrame[1]][2],
            pts[referenceFrame[2]][0], pts[referenceFrame[2]][1],
            pts[referenceFrame[2]][2], pts[referenceFrame[3]][0],
            pts[referenceFrame[3]][1], pts[referenceFrame[3]][2]
        );
        
        for (int i = 0; i<J.n();i++)
        {
            for (int j = 0; j < J.n(); j++)
            {
                if (i==j)
                    REQUIRE(Approx(J(i,j)) == i+1);
                else
                    REQUIRE(Approx(J(i,j)) == 0);
            }
        }
        
        WHEN("Inverse of Jacobi is correct")
        {
            // Check the inverse
            scalarSquareMatrix JInv = geometryWENO::JacobiInverse(J);
            
                for (int i = 0; i<J.n();i++)
            {
                for (int j = 0; j < J.n(); j++)
                {
                    if (i==j)
                        REQUIRE(Approx(JInv(i,j)*J(i,j)) == 1.0);
                    else
                        REQUIRE(Approx(JInv(i,j)) == 0);
                }
            }
            THEN("Check geometryWENO::transformPoint")
            {
                // Check transform point
                const point x0(1,0,0);
                const point xp(2,2,3);
                const point res = geometryWENO::transformPoint(JInv,xp,x0);
                
                REQUIRE(Approx(res[0])==1);
                REQUIRE(Approx(res[1])==1);
                REQUIRE(Approx(res[2])==1);
            }
        }
    }

}


TEST_CASE("geometryWENO::gaussQuad")
{
    //- Check the geometryWENO::gaussQuad function
    //  This function uses the gaussian integration with the order 5 
    //  Note: The area is not included in this function
    
    GIVEN("3 vectors for standard triangle 2D")
    {
        Foam::vector v0(0,0,0);
        Foam::vector v1(0,1,0);
        Foam::vector v2(1,0,0);
        
        vector vn = (v1 - v0) ^ (v2 - v0);
        
        REQUIRE(vn[0] == 0);
        REQUIRE(vn[1] == 0);
        REQUIRE(vn[2] == -1);
        
        scalar area = 0.5*mag(vn);
        
        REQUIRE(area == 0.5);
        
        THEN("Calculate Gauss quadrature")
        {            
            // Reference point p0
            point p0(0.333,0.333,1);
            
            scalar Int = geometryWENO::gaussQuad(1,1,1,p0,v0,v1,v2);
            
            // The result to check is obtained from matlab
            REQUIRE(Approx(Int) == 0.0277777);
            
        }
    }
    
    
}

//TEST_CASE("geometryWENO: Integration")
//{
    //// Replace setRootCase.H for Catch2   
    //int argc = 1;
    //char **argv = static_cast<char**>(malloc(sizeof(char*)));
    //char executable[] = {'m','a','i','n'};
    //argv[0] = executable;
    //Foam::argList args(argc, argv,false,false,false);
        
    //// create the mesh from case file
    //#include "createTime.H"
    //#include "createMesh.H"
    //#include "createControl.H"
    
    //geometryWENO geo;
    
    //using scalarMatrix = List< List< List<scalar> > > ;
    //using scalarSquareMatrix = SquareMatrix<scalar>;
    
    ////const fvMesh& mesh,
    //const label cellI = 33;
    //const label polOrder = 2;
    //scalarMatrix Integral;
    //scalarSquareMatrix JInvI;
    //point refPointI;
    //scalar refDetI;
    
    //geo.initIntegrals(mesh,cellI,polOrder,Integral,JInvI,refPointI,refDetI);
    
    
    
//}


