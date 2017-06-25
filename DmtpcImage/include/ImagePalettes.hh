#ifndef DMTPC_IMAGE_PALETTES_HH
#define DMTPC_IMAGE_PALETTES_HH


namespace dmtpc
{
  namespace image
  {
    namespace palette
    {
      
      //Monochromatic Schemes
      void setGrayscale(double p = 1);
      void setGreen(double p = 1);
      void setBlue(double p = 1);
      void setRed(double p = 1);
      void setCyan(double p = 1);
      void setMagenta(double p = 1);
      void setYellow(double p = 1);
      //Similar to ROOT palette 1
      void setStandard1();
      //Found in ROOT documentation somewhere
      void setPalette1();//Blue
      void setPalette2();//Pink/green
      void setPalette3();//green/pink
      //Matlab Color Schemes
      void setHot();
      void setCool(double p =1);
      void setJet();
      void setCopper();
      void setBone();
      void setWinter(double p = 1);
      void setSpring(double p = 1);
      void setSummer(double p = 1);
      void setAutumn(double p = 1);
      //Random other schemes (school colors, whatever else we want)
      void setMIT(double p = 1);
      void setColumbia(double p = 1);
      void setBU(double p = 1);

      //Random colors
      void setRandom(int n = 5);



    }
  }
}

#endif
