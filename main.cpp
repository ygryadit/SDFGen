//SDFGen - A simple grid-based signed distance field (level set) generator for triangle meshes.
//Written by Christopher Batty (christopherbatty@yahoo.com, www.cs.columbia.edu/~batty)
//...primarily using code from Robert Bridson's website (www.cs.ubc.ca/~rbridson)
//This code is public domain. Feel free to mess with it, let me know if you like it.

#include "makelevelset3.h"
#include "config.h"

//#ifdef HAVE_VTK
//  #include <vtkImageData.h>
//  #include <vtkFloatArray.h>
//  #include <vtkXMLImageDataWriter.h>
//  #include <vtkPointData.h>
//  #include <vtkSmartPointer.h>
//#endif


#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>

void rescaleTheShape(const Vec3f min_box, const Vec3f max_box, 
                    float min_box_dim, float max_box_dim, 
                    std::vector<Vec3f>& vertList)
{
    // Keep a small margin from min_box_dim and max_box_dim
    float padded_dim = 1e-5 * ((double)max_box_dim - (double)min_box_dim);

    Vec3f center = (max_box + min_box) / 2.0;
    
    std::cout << "center = " << center << std::endl;

    float max_dim = max(max_box - min_box);

    double scale = padded_dim / max_dim;
    std::cout << "scale = " << scale << std::endl;

    for (unsigned int t = 0; t < vertList.size(); ++t) {
        //center
        std::cout << "vert = " << vertList[t] << std::endl;
        vertList[t] = (vertList[t] - center) * scale;
        std::cout << "vertScaled = " << vertList[t] << std::endl;
    }
}

int main(int argc, char* argv[]) {
  
  
    std::cout << "SDFGen - A utility for converting closed oriented triangle meshes into grid-based signed distance fields.\n";
    std::cout << "\nThe output file format is:";
    std::cout << "<ni> <nj> <nk>\n";
    std::cout << "<origin_x> <origin_y> <origin_z>\n";
    std::cout << "<dx>\n";
    std::cout << "<value_1> <value_2> <value_3> [...]\n\n";
    
    std::cout << "(ni,nj,nk) are the integer dimensions of the resulting distance field.\n";
    std::cout << "(origin_x,origin_y,origin_z) is the 3D position of the grid origin.\n";
    std::cout << "<dx> is the grid spacing.\n\n";
    std::cout << "<value_n> are the signed distance data values, in ascending order of i, then j, then k.\n";

    std::cout << "The output filename will match that of the input, with the OBJ suffix replaced with SDF.\n\n";

    std::cout << "Usage: SDFGen <filename> <dx> <padding>\n\n";
    std::cout << "Where:\n";
    std::cout << "\t<filename> specifies a Wavefront OBJ (text) file representing a *triangle* mesh (no quad or poly meshes allowed). File must use the suffix \".obj\".\n";
    std::cout << "\t<min_box_dim> specifies the minimum dimension of the grid bounding box.\n";
    std::cout << "\t<max_box_dim> specifies the maximum dimension of the grid bounding box.\n";    
    std::cout << "\t<grid_size> specifies the maximum dimension of the grid bounding box.\n";
    
    if (argc != 5) {
        exit(-1);
    }

    //Check that the first file is in 'obj' format.
    std::string filename(argv[1]);
    if(filename.size() < 5 || filename.substr(filename.size()-4) != std::string(".obj")) {
        std::cerr << "Error: Expected OBJ file with filename of the form <name>.obj.\n";
        exit(-1);
    }

    //Read bounding box dimensions:
    std::stringstream arg2(argv[2]);
    float min_box_dim;
    arg2 >> min_box_dim;

    std::stringstream arg3(argv[3]);
    float max_box_dim;
    arg3 >> max_box_dim;


    std::stringstream arg4(argv[4]);
    float grid_size;
    arg4 >> grid_size;
    
    


    float dx;
     
    
    //The varaiable for the object actual bounding box:
    Vec3f   min_box(std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),std::numeric_limits<float>::max()), 
            max_box(-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max());
  
    std::cout << "Reading data.\n";

    std::ifstream infile(argv[1]);
    if(!infile) {
        std::cerr << "Failed to open. Terminating.\n";
        exit(-1);
    }

  int ignored_lines = 0;
  std::string line;
  std::vector<Vec3f> vertList;
  std::vector<Vec3ui> faceList;
  while(!infile.eof()) {
    std::getline(infile, line);

    //.obj files sometimes contain vertex normals indicated by "vn"
    if(line.substr(0,1) == std::string("v") && line.substr(0,2) != std::string("vn")){
      std::stringstream data(line);
      char c;
      Vec3f point;
      data >> c >> point[0] >> point[1] >> point[2];
      vertList.push_back(point);
      update_minmax(point, min_box, max_box);
    }
    else if(line.substr(0,1) == std::string("f")) {
      std::stringstream data(line);
      char c;
      int v0,v1,v2;
      data >> c >> v0 >> v1 >> v2;
      faceList.push_back(Vec3ui(v0-1,v1-1,v2-1));
    }
    else if( line.substr(0,2) == std::string("vn") ){
      std::cerr << "Obj-loader is not able to parse vertex normals, please strip them from the input file. \n";
      exit(-2); 
    }
    else if (line.substr(0, 2) == std::string("vt")) {
        std::cerr << "Obj-loader is not able to parse vertex textures, please strip them from the input file. \n";
        exit(-2);
    }
    else {
      ++ignored_lines; 
    }
  }
  infile.close();
  
  if(ignored_lines > 0)
        std::cout << "Warning: " << ignored_lines << " lines were ignored since they did not contain faces or vertices.\n";

  std::cout << "Read in " << vertList.size() << " vertices and " << faceList.size() << " faces." << std::endl;

  std::cout << "Shape bound box size: (" << min_box << ") to (" << max_box << ") dim sum = " << max_box - min_box << std::endl;

  




  //Add padding around the box.
  //Vec3f unit(1,1,1);
  Vec3f min_box_(min_box_dim, min_box_dim, min_box_dim);
  Vec3f max_box_(max_box_dim, max_box_dim, max_box_dim);

  rescaleTheShape(min_box, max_box,
      min_box_dim, max_box_dim,
      vertList);


  /*if ((min_box[0] < min_box_[0]) | (min_box[1] < min_box_[1]) | (min_box[2] < min_box_[2]) |
      (max_box[0] > max_box_[0]) | (max_box[1] > max_box_[1]) | (max_box[2] < max_box_[2]))
      std::cerr << "The shape is assumed to be normalised to have the digonal equal to 1" << std::endl;*/

  /*min_box -= padding*dx*unit;
  max_box += padding*dx*unit;*/


  min_box = min_box_;
  max_box = max_box_;


  /*Vec3ui sizes = Vec3ui((max_box - min_box)/dx);*/
  
  Vec3ui sizes(grid_size, grid_size, grid_size);

  dx = (max_box[0] - min_box[0]) / sizes[0];
  


  std::cout << "Bound box size: (" << min_box << ") to (" << max_box << ") with dimensions " << sizes << "." << std::endl;

  std::cout << "Computing signed distance field.\n";
  Array3f phi_grid;
  Array3f grid_x, grid_y, grid_z;

  //
  //grid_x.resize(sizes[0], sizes[1], sizes[2]);
  //grid_y.resize(sizes[0], sizes[1], sizes[2]);
  //grid_z.resize(sizes[0], sizes[1], sizes[2]);

   make_level_set3(faceList, vertList, min_box, dx, sizes[0], sizes[1], sizes[2], phi_grid);
  /*make_level_set3(faceList, vertList, min_box, dx, sizes[0], sizes[1], sizes[2], phi_grid);*/
  //make_level_set3(faceList, vertList, min_box, dx, sizes[0], sizes[1], sizes[2]);

  std::cout << "Computed lelvel sets: "  << "\n";

  std::string outname;

  //#ifdef HAVE_VTK
  //  // If compiled with VTK, we can directly output a volumetric image format instead
  //  //Very hackily strip off file suffix.
  //  outname = filename.substr(0, filename.size()-4) + std::string(".vti");
  //  std::cout << "Writing results to: " << outname << "\n";
  //  vtkSmartPointer<vtkImageData> output_volume = vtkSmartPointer<vtkImageData>::New();

  //  output_volume->SetDimensions(phi_grid.ni ,phi_grid.nj ,phi_grid.nk);
  //  output_volume->SetOrigin( phi_grid.ni*dx/2, phi_grid.nj*dx/2,phi_grid.nk*dx/2);
  //  output_volume->SetSpacing(dx,dx,dx);

  //  vtkSmartPointer<vtkFloatArray> distance = vtkSmartPointer<vtkFloatArray>::New();
  //  
  //  distance->SetNumberOfTuples(phi_grid.a.size());
  //  
  //  output_volume->GetPointData()->AddArray(distance);
  //  distance->SetName("Distance");

  //  for(unsigned int i = 0; i < phi_grid.a.size(); ++i) {
  //    distance->SetValue(i, phi_grid.a[i]);
  //  }

  //  vtkSmartPointer<vtkXMLImageDataWriter> writer =
  //  vtkSmartPointer<vtkXMLImageDataWriter>::New();
  //  writer->SetFileName(outname.c_str());

  //  #if VTK_MAJOR_VERSION <= 5
  //    writer->SetInput(output_volume);
  //  #else
  //    writer->SetInputData(output_volume);
  //  #endif
  //  writer->Write();

  //#else
    // if VTK support is missing, default back to the original ascii file-dump.
    //Very hackily strip off file suffix.

    outname = filename.substr(0, filename.size()-4) + std::string(".sdf");


    std::cout << "Writing results to: " << outname << "\n";
    
    std::ofstream outfile( outname.c_str());
    outfile << phi_grid.ni << " " << phi_grid.nj << " " << phi_grid.nk << std::endl;
    outfile << min_box[0] << " " << min_box[1] << " " << min_box[2] << std::endl;
    outfile << dx << std::endl;
    for(unsigned int i = 0; i < phi_grid.a.size(); ++i) {
      outfile << phi_grid.a[i] << std::endl;
    }
    outfile.close();


    //Save positions:
    grid_x.resize(sizes[0], sizes[1], sizes[2]);
    grid_y.resize(sizes[0], sizes[1], sizes[2]);
    grid_z.resize(sizes[0], sizes[1], sizes[2]);


    for (int k = 0; k < sizes[2]; ++k) for (int j = 0; j < sizes[1]; ++j) for (int i = 0; i < sizes[0]; ++i) {
        Vec3f gx(i * dx + min_box[0], j * dx + min_box[1], k * dx + min_box[2]); // not the center of a grid.          

        grid_x(i, j, k) = gx[0];
        grid_y(i, j, k) = gx[1];
        grid_z(i, j, k) = gx[2];

    }


    std::string outname_grid_pos = filename.substr(0, filename.size() - 4) + std::string("_pos.txt");
    
    std::ofstream outfile_pos(outname_grid_pos.c_str());

    for (unsigned int i = 0; i < phi_grid.a.size(); ++i) {
        outfile_pos << grid_x.a[i] << " " << grid_y.a[i] << " "  << grid_z.a[i] << std::endl;
    }
    outfile_pos.close();


  //#endif

  std::cout << "Processing complete.\n";

return 0;
}
