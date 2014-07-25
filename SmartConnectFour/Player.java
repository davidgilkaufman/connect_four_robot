/*
 * Player.java
 *
 * Created on December 5, 2003, 11:55 PM
 */

/**
 *
 * @author  Chen
 */
public interface Player {
    public int go(SimpleBoard b);
    public void setMove(int col);
    public int getType();
}
