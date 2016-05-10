package cz.vutbr.fit.fyo.diffraction.apertures;

import org.apache.commons.math3.analysis.function.Sinc;

/**
 * Class for computation of diffraction of rectangular aperture
 * @author Pavel Frýz a Lukáš Němec
 */
public class Rectangle{
    private static final Sinc sinc = new Sinc();
    private double width;
    private double height;
    private double distance;
    private double wavelenght;
    private double coef1;
    private double coef2;

    public Rectangle(double width, double height, double distance, double wavelenght) {
        this.width = width;
        this.height = height;
        this.distance = distance;
        this.wavelenght = wavelenght;
        updateCoef();
    }
    
    public double getWidth() {
        return width;
    }

    public double getHeight() {
        return height;
    }
    
    public double getDistance() {
        return distance;
    }

    public double getWavelenght() {
        return wavelenght;
    }

    public void setWidth(double width) {
        this.width = width;
        updateCoef();
    }

    public void setHeight(double height) {
        this.height = height;
        updateCoef();
    }
    
    public void setDistance(double distance) {
        this.distance = distance;
        updateCoef();
    }

    public void setWavelenght(double wavelenght) {
        this.wavelenght = wavelenght;
        updateCoef();
    }

    private void updateCoef() {
        coef1 = (Math.PI*width)/(wavelenght*distance);
        coef2 = (Math.PI*height)/(wavelenght*distance);
    }
    
    public double minX(){
        return (wavelenght*distance)/width;
    }
    
    public double minY(){
         return (wavelenght*distance)/height;
    }
    
    public double intensity(double x, double y) {
        double sqint = sinc.value(coef1*x)*sinc.value(coef2*y); 
        return sqint*sqint;
    }
}
