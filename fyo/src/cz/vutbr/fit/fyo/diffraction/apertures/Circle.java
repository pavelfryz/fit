package cz.vutbr.fit.fyo.diffraction.apertures;

import org.apache.commons.math3.special.BesselJ;

/**
 * Class for computation of diffraction of circle aperture
 * @author Pavel Frýz a Lukáš Němec
 */
public class Circle{
    private static final BesselJ J1 = new BesselJ(1);
    private static final double JZ1 = 3.831705970207512315614435886308;
    private double radius;
    private double distance;
    private double wavelenght;
    private double coef;

    public Circle(double radius, double distance, double wavelenght) {
        this.radius = radius;
        this.distance = distance;
        this.wavelenght = wavelenght;
        updateCoef();
    }
    
    public double getRadius() {
        return radius;
    }

    public double getDistance() {
        return distance;
    }

    public double getWavelenght() {
        return wavelenght;
    }

    public void setDistance(double distance) {
        this.distance = distance;
        updateCoef();
    }

    public void setRadius(double radius) {
        this.radius = radius;
        updateCoef();
    }

    public void setWavelenght(double wavelenght) {
        this.wavelenght = wavelenght;
        updateCoef();
    }

    private void updateCoef() {
        coef = (2*Math.PI*radius)/(wavelenght*distance);
    }
    
    public double min(){
        return JZ1/coef;
    }
    
    public double intensity(double radius) {
        double rcoef = radius*coef;
        double sqint = (rcoef==0) ? 1 : 2*J1.value(1,rcoef)/rcoef;
        return sqint*sqint;
    }
    
    public double intensity(double x, double y) {
        return intensity(Math.sqrt(x*x+y*y));
    }   
}
