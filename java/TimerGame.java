import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;

public class TimerGame extends JFrame {
	final int time=10;
	JButton start;
	JButton stop;
	JButton average;
	JButton clear;
	JLabel score;
	long tstart;
	long tstop;
	ArrayList<Double> scoreArray;
	Boolean state;

	public static void main(String[] args){
		TimerGame tg = new TimerGame("10 Sec Game");
		tg.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		tg.setSize(350, 150);
		tg.setVisible(true);
	}

	TimerGame(String title){
		setTitle(title);		//super(title);
		JPanel panel = new JPanel();
		getContentPane().add(panel);
		start = new JButton("start");
		stop = new JButton("stop");
		average = new JButton("average");
		clear = new JButton("clear");
		score = new JLabel("");
		tstart=0;
		tstop=0;
		scoreArray = new ArrayList<Double> ();
		state = true;
		panel.add(start);
		panel.add(stop);
		panel.add(average);
		panel.add(clear);
		panel.add(score);

		start.addActionListener(new StartListener());
		stop.addActionListener(new StopListener());
		average.addActionListener(new AveListener());
		clear.addActionListener(new ClearListener());
	}
	class StartListener implements ActionListener{
		public void actionPerformed(ActionEvent e){
			if(state){
				tstart=System.currentTimeMillis();
				state=false;
			}
		}
	}
	class StopListener implements ActionListener{
		public void actionPerformed(ActionEvent e){
			if(!state){
				tstop=System.currentTimeMillis();
				state=true;
				double ttime = (double)(tstop-tstart)/1000-time;
				scoreArray.add(ttime);
				score.setText(String.valueOf(ttime)+"秒のずれです");
			}
		}
	}
	class AveListener implements ActionListener{
		public void actionPerformed(ActionEvent e){
			if(scoreArray.size()!=0){
				double sum=0;
				for(double i:scoreArray){
					sum+=i;
				}
				score.setText("平均"+String.valueOf(sum/scoreArray.size())+"秒のずれです");
			}
			else{
				score.setText("履歴がありません");
			}
		}
	}
	class ClearListener implements ActionListener{
		public void actionPerformed(ActionEvent e){
			scoreArray.clear();
			score.setText("履歴を削除しました");
		}
	}
}